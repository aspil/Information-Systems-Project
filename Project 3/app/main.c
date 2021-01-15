#include "../include/classifier/classifier.h"
#include "../include/classifier/classifier_utils.h"
#include "../include/classifier/metrics.h"
#include "../include/clique/clique.h"
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

double learning_rate;

void free_data(struct labels_sets *s);

int main(int argc, char *argv[])
{
	srand(time(NULL));
	char *data_path = NULL;
	char *relations_file = NULL;
	int	  max_features = 0;
	int	  debug = -1;

	int mode = parse_cmd_arguments(argc, argv, &data_path, &relations_file, &max_features,
								   &learning_rate, &debug);

	int size = count_json_files(data_path);

	// if ((size = pick_the_buckets_and_features(argc, argv, &max_features, &print)) == -1)
	// 	return -1;
	if (mode == TRN_MD) {
		/* Safe assignments, error checking was done in pick_the_buckets */
		// char *data_path = argv[1];
		// char *relations_file = argv[2];
		// learning_rate = 0.05;
		/* Construct the stopwords dictionary */
		stopwords = map_init(37, hash_str, compare_str, free, NULL);
		get_stopwords("data/misc/stopwords.txt");

		/* Initialize the hash table */
		struct hash_map *map = map_init(size, hash_str, compare_str, free, delete_clique);

		read_data_files(map, size, data_path);

		read_relations(map, relations_file);

		make_the_files(map);

		map_delete(map);

		int				   n_files = count_json_files(data_path);
		struct vectorizer *tfidf = vectorizer_init(n_files, 1);	   // 1 means tfidf instead of bow
		vectorizer_fit_transform(tfidf, data_path, max_features);

		printf("Reading the binary labels...\n");

		int n_positive_labels, n_negative_labels;
		if ((n_positive_labels = get_label_count("data/train/positive_relations.csv")) < 0)
			exit(EXIT_FAILURE);

		char **positive_labels = malloc(n_positive_labels * sizeof(char *));

		if ((n_negative_labels = get_label_count("data/train/negative_relations.csv")) < 0)
			exit(EXIT_FAILURE);

		char **negative_labels = malloc(n_negative_labels * sizeof(char *));

		if ((load_labels("data/train/positive_relations.csv", positive_labels,
						 "data/train/negative_relations.csv", negative_labels)) < 0)
			exit(EXIT_FAILURE);

		struct labels_sets *sets;
		if ((sets = train_test_split(positive_labels, n_positive_labels, negative_labels,
									 n_negative_labels, 0.6, 0.2, 1)) == NULL)
			exit(EXIT_FAILURE);

		printf("Initializing the logistic regression model\n");

		struct LogisticRegressor *model = Logistic_Regression_Init();
		Logistic_Regression_fit(model, tfidf);

		printf("Training the model...\n");
		train(model, sets->train_set, sets->n_train_labels);

		// write the weights in a separate file
		printf("Saving the model's weights to a file\n");
		FILE *fp;
		if ((fp = fopen("data/model/weights.txt", "w")) == NULL) {
			perror("main: can't open weigts.txt:");
			exit(EXIT_FAILURE);
		}
		fprintf(fp, "%d\n", model->n_weights);
		for (int i = 0; i < model->n_weights; ++i)
			fprintf(fp, "%f\n", model->weights[i]);

		fclose(fp);

		// write the testing labels to a separate file
		FILE *fp_1;
		if ((fp_1 = fopen("data/test/testing_labels.csv", "w")) == NULL) {
			perror("main: can't open weigts.txt:");
			exit(EXIT_FAILURE);
		}
		fprintf(fp_1, "%d\n", sets->n_test_labels);
		for (int i = 0; i < sets->n_test_labels; ++i)
			fprintf(fp_1, "%s%d\n", sets->test_set_input[i], sets->test_set_labels[i]);

		fclose(fp_1);

		print_cliques(debug);

		Logistic_Regression_Delete(model);

		/* Free the allocated memory */
		free_data(sets);
		vectorizer_delete(tfidf);

		map_delete(stopwords);
	}
	else if (mode == TST_MD) {
		// char *data_path = "data";

		int n_files = count_json_files(data_path);

		stopwords = map_init(37, hash_str, compare_str, free, NULL);
		get_stopwords("data/misc/stopwords.txt");

		struct LogisticRegressor *model = Logistic_Regression_Init();

		printf("Fetching the model's weights from data/model/weights.txt\n");
		FILE *fp;
		if ((fp = fopen("data/model/weights.txt", "r")) == NULL) {
			perror("main: couldn't open data/model/weights.txt:");
			fprintf(stderr, "Please make sure that you have ran the app using training options\n");
			exit(EXIT_FAILURE);
		}
		int		i = 0;
		char *	line = NULL;
		size_t	len = 0;
		ssize_t read;

		if ((read = getline(&line, &len, fp)) < 0) {
			perror("main: error in getline\n");
			exit(EXIT_FAILURE);
		}
		model->n_weights = atoi(line);
		model->weights = malloc(sizeof(double) * model->n_weights);

		while ((read = getline(&line, &len, fp)) != -1) {
			model->weights[i] = atof(line);	   // pass the weights from training
			i++;
		}
		fclose(fp);

		struct vectorizer *tfidf = vectorizer_init(n_files, 1);
		vectorizer_fit_transform(tfidf, data_path, (model->n_weights - 1) / 2);
		model->vect = tfidf;

		if ((fp = fopen("data/test/testing_labels.csv", "r")) == NULL) {
			perror("main: couldn't open data/test/testing_labels.csv\n");
			exit(EXIT_FAILURE);
		}

		if ((read = getline(&line, &len, fp)) < 0) {
			perror("main: error in getline at data/test/testing_labels.csv\n");
			exit(EXIT_FAILURE);
		}

		int	   n_test_labels = atoi(line);
		int *  test_labels = malloc(n_test_labels * sizeof(int));
		char **test_set = malloc(n_test_labels * sizeof(char *));

		i = 0;
		while ((read = getline(&line, &len, fp)) != -1) {
			get_line_without_end_line(line);	// take the relations
			test_set[i] = malloc(strlen(line) + 1);
			strcpy(test_set[i], line);
			test_labels[i] = line[strlen(line) - 1] - '0';
			i++;
		}
		fclose(fp);

		printf("Testing the model...\n");
		int *predictions = test(model, test_set, n_test_labels);

		printf("Accuracy: %f\n", accuracy_score(test_labels, predictions, n_test_labels));
		printf("Precision: %f\n", precision_score(test_labels, predictions, n_test_labels));
		printf("Recall: %f\n", recall_score(test_labels, predictions, n_test_labels));
		printf("F1: %f\n", f1_score(test_labels, predictions, n_test_labels));

		for (int i = 0; i < n_test_labels; ++i)
			free(test_set[i]);

		free(line);
		free(predictions);
		free(test_set);
		free(test_labels);

		map_delete(stopwords);
		vectorizer_delete(tfidf);
		Logistic_Regression_Delete(model);
	}
	else {	  // validate area
		printf("Welcome to the validate area\n");

		// char *data_path = "data";

		int size = count_json_files(data_path);

		FILE *fp;
		if ((fp = fopen("data/model/weights.txt", "r")) == NULL) {
			perror("main: couldn't open data/model/weights.txt:");
			exit(EXIT_FAILURE);
		}
		int		i = 0;
		char *	line = NULL;
		size_t	len = 0;
		ssize_t read;
		if ((read = getline(&line, &len, fp)) < 0) {
			perror("main: error in getline at data/model/weights.txt\n");
			exit(EXIT_FAILURE);
		}

		struct LogisticRegressor *model = Logistic_Regression_Init();
		model->n_weights = atoi(line);
		model->weights = malloc(sizeof(double) * model->n_weights);

		stopwords = map_init(37, hash_str, compare_str, free, NULL);
		get_stopwords("data/misc/stopwords.txt");

		struct vectorizer *tfidf = vectorizer_init(size, 1);
		vectorizer_fit_transform(tfidf, data_path, (model->n_weights - 1) / 2);
		model->vect = tfidf;

		map_delete(stopwords);

		int *test_labels, n_test_labels;
		while ((read = getline(&line, &len, fp)) != -1) {
			model->weights[i] = atof(line);	   // pass the weights from training
			i++;
		}
		fclose(fp);

		int j = 0;

		i = 0;

		if ((fp = fopen("data/initial_labels/positive_relations.csv", "r")) != NULL) {
			perror("main: Cannot open data/initial_labels/positive_relations.csv:");
			exit(EXIT_FAILURE);
		}

		while ((read = getline(&line, &len, fp)) != -1)
			i++;

		fclose(fp);

		if ((fp = fopen("data/initial_labels/negative_relations.csv", "r")) != NULL) {
			perror("main: Cannot open data/initial_labels/negative_relations.csv:");
			exit(EXIT_FAILURE);
		}
		while ((read = getline(&line, &len, fp)) != -1)
			j++;

		fclose(fp);

		if ((fp = fopen("data/initial_labels/positive_relations.csv", "r")) != NULL) {
			perror("main: Cannot open data/initial_labels/positive_relations.csv:");
			exit(EXIT_FAILURE);
		}

		n_test_labels = j + i;

		char **test_set = malloc(sizeof(char *) * n_test_labels);
		test_labels = malloc(sizeof(int) * n_test_labels);

		int counter = 0;
		while ((read = getline(&line, &len, fp)) != -1) {
			get_line_without_end_line(line);	// take the relations
			test_set[counter] = malloc(strlen(line) + 1);
			strcpy(test_set[counter], line);
			test_labels[counter] = line[strlen(line) - 1] - '0';
			counter++;
		}
		fclose(fp);

		if ((fp = fopen("data/initial_labels/negative_relations.csv", "r")) != NULL) {
			perror("main: Cannot open data/initial_labels/negative_relations.csv:");
			exit(EXIT_FAILURE);
		}
		while ((read = getline(&line, &len, fp)) != -1) {
			get_line_without_end_line(line);	// take the relations
			test_set[counter] = malloc(strlen(line) + 1);
			strcpy(test_set[counter], line);
			test_labels[counter] = line[strlen(line) - 1] - '0';
			counter++;
		}
		fclose(fp);

		int *predictions = test(model, test_set, n_test_labels);

		printf("Accuracy: %f\n", accuracy_score(test_labels, predictions, n_test_labels));
		printf("Precision: %f\n", precision_score(test_labels, predictions, n_test_labels));
		printf("Recall: %f\n", recall_score(test_labels, predictions, n_test_labels));
		printf("F1: %f\n", f1_score(test_labels, predictions, n_test_labels));
		free(predictions);
		Logistic_Regression_Delete(model);
		vectorizer_delete(tfidf);

		for (int i = 0; i < n_test_labels; ++i)
			free(test_set[i]);

		free(line);
		free(test_set);
		free(test_labels);
	}
	return 0;
}

void free_data(struct labels_sets *s)
{
	for (int i = 0; i < s->n_train_labels; ++i)
		free(s->train_set[i]);

	free(s->train_set);

	for (int i = 0; i < s->n_test_labels; ++i)
		free(s->test_set_input[i]);

	free(s->test_set_input);
	free(s->test_set_labels);

	for (int i = 0; i < s->n_validate_labels; ++i)
		free(s->validate_set[i]);

	free(s->validate_set);

	free(s);
}

/*
struct all_info *start=NULL;

start=malloc(sizeof(struct all_info));
start->size=0;
start->first=NULL;

read_relations2(map,relations_file,start);
int l = print_results(map);
printf("THe integer is %d \n",l);

struct list_negative *tranverse=start->first;

int ok=print_negative_results(map);

printf("%d\n", ok);
int sum=0;

while (tranverse!=NULL)
{
	printf("%d %d \n",tranverse->ptr_1->size,tranverse->ptr_2->size );
	sum=sum + (tranverse->ptr_1->size * tranverse->ptr_2->size);
	tranverse=tranverse->next;
}

printf("Sum is %d \n",sum );
*/