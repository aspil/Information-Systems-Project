#include "../include/classifier/classifier.h"
#include "../include/classifier/classifier_utils.h"
#include "../include/classifier/metrics.h"
#include "../include/clique/clique.h"
#include "../include/scheduler/scheduler.h"
#include "../include/util/argparse.h"
#include "../include/util/dataset_parsing.h"
#include "../include/util/text_preprocessing.h"
#include "../include/util/util.h"
#include "../include/word_embeddings/vectorizer.h"
#include "../modules/hashtable/map.h"
#include "../modules/list/list.h"

#include <dirent.h>
#include <errno.h>
#include <float.h>
#include <getopt.h>
#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

struct hash_map *stopwords;

double *min_weights;
double	min_loss;

void free_data(Datasets *s);

int main(int argc, char *argv[])
{
	srand(time(NULL));
	char * data_path = NULL;
	char * relations_file = NULL;
	int	   max_features = 0;
	int	   debug = -1;
	double learning_rate = 0.5;
	int	   n_threads;
	int	   n_epochs = 1;
	int	   batch_size = 512;

	int stratify;

	int mode = parse_cmd_arguments(argc, argv, &data_path, &relations_file, &max_features, &learning_rate, &n_threads,
								   &batch_size, &n_epochs, &debug, &stratify);

	double predict_threshold = (stratify == 1) ? 0.5 : 0.49;

	int total_data_files = count_json_files(data_path);
	if (mode < 0) {
		fprintf(stderr, "./bin/app: Error on -m option; invalid argument");
		exit(EXIT_FAILURE);
	}
	else if (mode == TRN_MD || mode == ITR_MD) {
		/* Initialize the hash table */
		struct hash_map *map = map_init(total_data_files, hash_str, compare_str, free, delete_clique);

		printf("Loading the dataset\n");
		read_data_files(map, total_data_files, data_path);

		printf("Constructing cliques and negative relations\n");
		read_relations(map, relations_file);

		make_the_files(map);

		map_delete(map);

		printf("Constructing the datasets...\n");
		Datasets *sets;

		if ((sets = train_test_split(0.6, 0.2, stratify)) == NULL)
			exit(EXIT_FAILURE);

		if (batch_size > sets->n_train) {
			printf("Batch size hyperparameter exceeds the train set's limits(%d).\nPlease provide a smaller batch size "
				   "or press Ctrl+D to exit: ",
				   sets->n_train);
			if (scanf("%d", &batch_size) == EOF) {
				printf("Exiting...\n");
				exit(EXIT_SUCCESS);
			}
		}
		stopwords = map_init(37, hash_str, compare_str, free, NULL);
		get_stopwords("data/misc/stopwords.txt");
		printf("Initializing the vectorizer\n");
		Vectorizer *tfidf = vectorizer_init(total_data_files, 1);	 // 1 means tfidf instead of bow

		printf("Transforming the data into tfidf word embedding\n");
		vectorizer_fit_transform(tfidf, data_path, max_features);
		printf("Initializing the logistic regression model\n");

		LogisticRegressor *model = Logistic_Regression_Init(learning_rate, n_epochs, batch_size, predict_threshold);
		Logistic_Regression_fit(model, sets, tfidf);

		min_weights = calloc(model->n_weights, sizeof(double));
		min_loss = 1000;
		if (mode == TRN_MD) {
			train(model, n_threads);
			printf("Exiting...\n");
		}
		else {
			char **files = malloc(total_data_files * sizeof(char *));
			int	   cnt = 0;
			get_json_files(data_path, cnt, files);
			double threshold = 0.1, step_value = 0.1, p, *x;

			struct hash_map *train_samples_map = map_init(model->datasets->n_train, hash_str, compare_str, NULL, NULL);
			for (int i = 0; i < model->datasets->n_train; i++) {
				map_insert(train_samples_map, model->datasets->train_samples[i], NULL);
			}
			while (threshold < 0.5) {
				struct vector *new_training_set = vector_init(2048, free);
				printf("Training\n");

				train(model, n_threads);

				char *document1 = NULL;
				char *document2 = NULL;
				parse_relation(model->datasets->train_samples[1], &document1, &document2);

				for (int i = 0; i < model->datasets->n_train; ++i) {
					parse_relation(model->datasets->train_samples[i], &document1, &document2);
					x = vectorizer_get_vector(model->vect, document1, document2);
					p = sigmoid(x, model->weights, model->n_weights);
				}

				for (int i = 0; i < total_data_files; i++) {
					// start=clock();
					for (int j = i + 1; j < total_data_files;
						 j++) {	   // Start from i+1 to avoid getting reverse relation
						// first i have to check whether they belong in the same clique
						// if they do from transitivity i dont need to examine them

						char *temp = malloc(strlen(files[i]) + 2 + strlen(files[j]) + 1);
						strcpy(temp, files[i]);
						strcat(strcat(strcat(temp, ","), files[j]), ",");

						if (map_find(train_samples_map, temp) == NULL) {
							x = vectorizer_get_vector(model->vect, files[i], files[j]);

							p = sigmoid(x, model->weights, model->n_weights);

							if (p < threshold) {
								char *pair = malloc(strlen(files[i]) + strlen(files[j]) + 10);
								// strcat(strcat(strcpy(pair, files[i]), files[j]), itoa(0));
								sprintf(pair, "%s,%s,%d", files[i], files[j], 0);

								vector_push_back(new_training_set, pair);
							}
							else if (p > 1 - threshold) {
								char *pair = malloc(strlen(files[i]) + strlen(files[j]) + 10);

								// strcat(strcat(strcpy(pair, files[i]), files[j]), itoa(1));
								sprintf(pair, "%s,%s,%d", files[i], files[j], 1);

								vector_push_back(new_training_set, pair);
							}
							free(x);
						}
					}
					break;
				}

				resolve_transitivity_issues(new_training_set, map);

				vector_delete(new_training_set);

				threshold += step_value;
			}
		}

		free_data(sets);
	}
	else if (mode == TST_MD) {
		FILE *	fp;
		char *	line = NULL;
		size_t	len = 0;
		ssize_t read;

		if ((fp = fopen("data/test/testing_data.csv", "r")) == NULL) {
			perror("main: couldn't open data/test/testing_data.csv\n");
			exit(EXIT_FAILURE);
		}
		if ((read = getline(&line, &len, fp)) < 0) {
			perror("main: error in getline at data/test/testing_data.csv\n");
			exit(EXIT_FAILURE);
		}
		int	   n_test_labels = atoi(line);
		char **test_samples = malloc(n_test_labels * sizeof(char *));
		int *  test_labels = malloc(n_test_labels * sizeof(int));

		int i = 0;
		while ((read = getline(&line, &len, fp)) != -1) {
			get_line_without_end_line(line);	// take the relations
			test_samples[i] = malloc(strlen(line) + 1);
			strcpy(test_samples[i], line);
			test_labels[i] = line[strlen(line) - 1] - '0';
			i++;
		}
		fclose(fp);

		if (batch_size > n_test_labels) {
			printf("Batch size hyperparameter exceeds the train set's limits(%d).\nPlease provide a smaller batch "
				   "size "
				   "or press Ctrl+D to exit: ",
				   n_test_labels);
			if (scanf("%d", &batch_size) == EOF) {
				printf("Exiting...\n");
				exit(EXIT_SUCCESS);
			}
			printf("%d\n", batch_size);
		}
		stopwords = map_init(37, hash_str, compare_str, free, NULL);

		get_stopwords("data/misc/stopwords.txt");
		printf("Initializing the vectorizer\n");

		if ((fp = fopen("data/model/weights.txt", "r")) == NULL) {
			perror("main: couldn't open data/model/weights.txt:");
			fprintf(stderr, "Please make sure that you have ran the app using training options\n");
			exit(EXIT_FAILURE);
		}
		if ((read = getline(&line, &len, fp)) < 0) {
			perror("main: error in getline\n");
			exit(EXIT_FAILURE);
		}
		int			n_weights = atoi(line);
		Vectorizer *tfidf = vectorizer_init(total_data_files, 1);	 // 1 means tfidf instead of bow

		printf("Transforming the data into tfidf word embedding\n");
		max_features = (n_weights - 1) / 2;
		vectorizer_fit_transform(tfidf, data_path, max_features);

		printf("Initializing the logistic regression model\n");
		LogisticRegressor *model = Logistic_Regression_Init(0, 0, batch_size, predict_threshold);

		Datasets *test_set = malloc(sizeof(struct Datasets));
		memset(test_set, 0, sizeof(struct Datasets));
		test_set->n_test = n_test_labels;
		test_set->test_samples = test_samples;
		test_set->test_labels = test_labels;

		Logistic_Regression_fit(model, test_set, tfidf);
		printf("Fetching the model's weights from data/model/weights.txt\n");
		i = 0;
		while ((read = getline(&line, &len, fp)) != -1) {
			model->weights[i] = atof(line);	   // get the weights from training
			i++;
		}
		fclose(fp);
		printf("Testing the model...\n");
		int *predictions = predict(model, n_threads);
		printf("Done testing\n");
		// for (int i = 0; i < model->datasets->n_test; i++) {
		// 	printf("%d ", predictions[i]);
		// }
		// 	printf("\n");
		printf("Accuracy: %f\n", accuracy_score(model->datasets->test_labels, predictions, model->datasets->n_test));
		printf("Precision: %f\n", precision_score(model->datasets->test_labels, predictions, model->datasets->n_test));
		printf("Recall: %f\n", recall_score(model->datasets->test_labels, predictions, model->datasets->n_test));
		printf("F1: %f\n", f1_score(model->datasets->test_labels, predictions, model->datasets->n_test));
	}
	else {
		printf("Wut\n");
	}
	return 0;
}

void free_data(Datasets *s)
{
	for (int i = 0; i < s->n_train; ++i)
		free(s->train_samples[i]);

	free(s->train_samples);
	free(s->train_labels);

	for (int i = 0; i < s->n_test; ++i)
		free(s->test_samples[i]);

	free(s->test_samples);
	free(s->test_labels);

	for (int i = 0; i < s->n_validate; ++i)
		free(s->validate_samples[i]);

	free(s->validate_samples);
	free(s->validate_labels);

	free(s);
}