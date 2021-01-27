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

	int mode = parse_cmd_arguments(argc, argv, &data_path, &relations_file, &max_features, &learning_rate, &n_threads,
								   &batch_size, &n_epochs, &debug);

	int total_data_files = count_json_files(data_path);

	if (mode == TRN_MD) {
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
		if ((sets = train_test_split(0.6, 0.2, 1)) == NULL)
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

		LogisticRegressor *model = Logistic_Regression_Init(learning_rate, n_epochs, batch_size);
		Logistic_Regression_fit(model, sets, tfidf);

		// for (int i = 0; i < model->datasets->n_train; i++)
		// 	printf("%s %d\n", model->datasets->train_samples[i], model->datasets->train_labels[i]);
		train(model, n_threads);
		char **files = malloc(total_data_files * sizeof(char *));
		int	   cnt = 0;
		get_json_files(data_path, cnt, files);
		// double threshold = 0.3, step_value = 0.05, p, *x;

		// while (threshold < 0.5) {
		// 	struct vector *new_training_set = vector_init(2048, free);
		// 	train(model, n_threads);
		// 	for (int i = 0; i < total_data_files; i++) {
		// 		for (int j = i + 1; j < total_data_files; j++) {	// Start from i+1 to avoid getting reverse relation

		// 			x = vectorizer_get_vector(model->vect, files[i], files[j]);
		// 			p = sigmoid(x, model->weights, model->n_weights);
		// 			if (p < threshold) {
		// 				char *pair = malloc(strlen(files[i]) + strlen(files[j]) + 2);
		// 				// strcat(strcat(strcpy(pair, files[i]), files[j]), itoa(0));
		// 				sprintf(pair, "%s,%s,%d", files[i], files[j], 0);
		// 				vector_push_back(new_training_set, pair);
		// 			}
		// 			else if (p > 1 - threshold) {
		// 				char *pair = malloc(strlen(files[i]) + strlen(files[j]) + 2);
		// 				// strcat(strcat(strcpy(pair, files[i]), files[j]), itoa(1));
		// 				sprintf(pair, "%s,%s,%d", files[i], files[j], 1);
		// 				vector_push_back(new_training_set, pair);
		// 			}
		// 			free(x);
		// 		}
		// 	}
		// 	resolve_transitivity_issues(model, new_training_set);
		// 	vector_delete(new_training_set);
		// 	threshold += step_value;
		// }

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
			printf("Batch size hyperparameter exceeds the train set's limits(%d).\nPlease provide a smaller batch size "
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
		LogisticRegressor *model = Logistic_Regression_Init(0, 0, batch_size);

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
		int *predictions = test(model, n_threads);
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