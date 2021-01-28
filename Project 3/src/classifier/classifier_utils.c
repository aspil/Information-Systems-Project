#include "../../include/classifier/classifier_utils.h"

#include "../../include/util/text_preprocessing.h"
#include "../../include/util/util.h"

#include <errno.h>
#include <fcntl.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

double *create_weights(int number_of_variables)
{
	double *array_of_weights = calloc(number_of_variables, sizeof(double));
	for (int i = 0; i < number_of_variables; ++i)
		array_of_weights[i] = 0.0;

	return array_of_weights;
}

#ifdef SHOWPROGRESS
int cnt;
int verb_labels;
#endif

int load_labels(char *positive_labels_file, char **positive_labels, char *negative_labels_file, char **negative_labels)
{
	size_t	len = 0;
	ssize_t read;
	char *	line = NULL;
	FILE *	fp;

	/* Load the positive labels */
	if ((fp = fopen(positive_labels_file, "r")) == NULL) {
		fprintf(stderr, "Error while opening '%s': ", positive_labels_file);
		return perror(""), -1;
	}

	int label_count = 0;

	while ((read = getline(&line, &len, fp)) != -1) {
		get_line_without_end_line(line);
		positive_labels[label_count] = malloc(strlen(line) + 1);
		strcpy(positive_labels[label_count], line);
		label_count++;
#ifdef SHOWPROGRESS
		printf("\r%.1f%%", rescale_lo_hi(cnt++, 0, verb_labels, 0, 100));
		fflush(stdout);
#endif
	}
	label_count = 0;
	fclose(fp);

	/* Now load the negative labels from the file */
	if ((fp = fopen(negative_labels_file, "r")) == NULL) {
		fprintf(stderr, "Error while opening '%s': ", positive_labels_file);
		return perror(""), -1;
	}

	while ((read = getline(&line, &len, fp)) != -1) {
		get_line_without_end_line(line);
		negative_labels[label_count] = malloc(strlen(line) + 1);
		strcpy(negative_labels[label_count], line);
		label_count++;
#ifdef SHOWPROGRESS
		printf("\r%.1f%%", rescale_lo_hi(cnt++, 0, verb_labels, 0, 100));
		fflush(stdout);
#endif
	}

#ifdef SHOWPROGRESS
	printf("%c[2K\rDone\n", 27);
#endif

	free(line);
	fclose(fp);
	return 0;
}

int get_label_count(char *labels_path)
{
	size_t	len = 0;
	ssize_t read;
	char *	line = NULL;
	int		n_labels = 0;
	FILE *	fp;

	if ((fp = fopen(labels_path, "r")) == NULL) {
		fprintf(stderr, "Error while opening '%s': ", labels_path);
		return perror(""), -1;
	}

	while ((read = getline(&line, &len, fp)) != -1)
		n_labels++;

	free(line);
	fclose(fp);
	return n_labels;
}

Datasets *train_test_split(double train_percent, double test_percent, int stratify)
{
#ifdef SHOWPROGRESS
	cnt = 0;
#endif
	int n_positive_labels, n_negative_labels;
	if ((n_positive_labels = get_label_count("data/train/positive_relations.csv")) < 0)
		exit(EXIT_FAILURE);

	if ((n_negative_labels = get_label_count("data/train/negative_relations.csv")) < 0)
		exit(EXIT_FAILURE);

#ifdef SHOWPROGRESS
	verb_labels = n_positive_labels + n_negative_labels;
#endif

	char **positive_labels = malloc(n_positive_labels * sizeof(char *));
	char **negative_labels = malloc(n_negative_labels * sizeof(char *));

	if ((load_labels("data/train/positive_relations.csv", positive_labels, "data/train/negative_relations.csv",
					 negative_labels)) < 0)
		exit(EXIT_FAILURE);

	double validate_percent = 1 - train_percent - test_percent;
	if (validate_percent <= 0) {
		fprintf(stderr, "Validation set percentage is negative.\nPlease make sure that the training and "
						"test proportions add up into a value strictly less than 1.\n");
		exit(EXIT_FAILURE);
	}
	/* Shuffle before sampling */
	shuffle_array(positive_labels, n_positive_labels, sizeof(char *));
	shuffle_array(negative_labels, n_negative_labels, sizeof(char *));
	Datasets *sets = malloc(sizeof(Datasets));
	if (stratify == 1) {
		/* Get the minimum count among the 0 and 1 labels. */
		int n_samples;

		if (n_positive_labels < n_negative_labels) { /* Sample the negative labels (0s) */
			n_samples = n_positive_labels;
			/* Shuffle before sampling */
			shuffle_array(negative_labels, n_negative_labels, sizeof(char *));
			/* Remove the excess negative samples */
			for (int i = n_samples; i < n_negative_labels; ++i)
				free(negative_labels[i]);
		}
		else { /* (n_negative_labels <= n_positive_labels) Sample the positive labels (1s) */
			n_samples = n_negative_labels;
			shuffle_array(positive_labels, n_positive_labels, sizeof(char *));
			/* Remove the excess negative samples */
			for (int i = n_samples; i < n_positive_labels; ++i)
				free(positive_labels[i]);
		}
		/* Shuffle again before splitting */
		shuffle_array(positive_labels, n_samples, sizeof(char *));
		shuffle_array(negative_labels, n_samples, sizeof(char *));

#ifdef SHOWPROGRESS
		cnt = 0;
#endif
		printf("Splitting the datasets into train, test and validate sets...\n");

		sets->n_train = train_percent * n_samples;
		sets->n_test = test_percent * n_samples;
		sets->n_validate = validate_percent * n_samples;

		/* Add the remaining labels to the training set */
		sets->n_train += n_samples - sets->n_train - sets->n_test - sets->n_validate;

		/* Create the training set array */
		construct_train_set(sets, positive_labels, negative_labels);

		/* Now construct the test set */
		construct_test_set(sets, positive_labels, negative_labels);

		/* Finally construct the validation set */
		construct_validation_set(sets, positive_labels, negative_labels);
		sets->n_train *= 2;
		sets->n_test *= 2;
		sets->n_validate *= 2;
#ifdef SHOWPROGRESS
		printf("%c[2K\rDone\n", 27);
#endif
		for (int i = 0; i < n_samples; ++i) {
			free(positive_labels[i]);
			free(negative_labels[i]);
		}
		free(positive_labels);
		free(negative_labels);
		return sets;
	}
	else {
		int	   n_samples = n_positive_labels + n_negative_labels;
		char **labels = malloc(n_samples * sizeof(char *));

		for (int i = 0; i < n_positive_labels; i++) {
			labels[i] = malloc(strlen(positive_labels[i]) + 1);
			strcpy(labels[i], positive_labels[i]);
			free(positive_labels[i]);
		}
		free(positive_labels);
		int i, j;
		for (i = n_positive_labels, j = 0; j < n_negative_labels; i++, j++) {
			labels[i] = malloc(strlen(negative_labels[j]) + 1);
			strcpy(labels[i], negative_labels[j]);
			free(negative_labels[j]);
		}
		free(negative_labels);
		i = 0, j = 0;
		sets->n_train = train_percent * n_samples;
		sets->n_test = test_percent * n_samples;
		sets->n_validate = validate_percent * n_samples;

		/* Add the remaining labels to the training set */
		sets->n_train += n_samples - sets->n_train - sets->n_test - sets->n_validate;

		sets->train_samples = malloc(sets->n_train * sizeof(char *));
		sets->train_labels = malloc(sets->n_train * sizeof(int));
		// int i, j;
		for (i = 0; i < sets->n_train; i++) {
			sets->train_samples[i] = malloc(strlen(labels[i]) + 1);

			strcpy(sets->train_samples[i], labels[i]);

			sets->train_labels[i] = sets->train_samples[i][strlen(sets->train_samples[i]) - 1] - '0';
			sets->train_samples[i][strlen(sets->train_samples[i]) - 1] = '\0';

			free(labels[i]);
#ifdef SHOWPROGRESS
			printf("\r%.1f%%", rescale_lo_hi(cnt++, 0, 2 * (sets->n_train + sets->n_test + sets->n_validate), 0, 100));
			fflush(stdout);
#endif
		}
		shuffle_arrays_similar(sets->train_samples, sets->train_labels, sets->n_train, sizeof(char *), sizeof(int));

		sets->test_samples = malloc(sets->n_test * sizeof(char *));
		sets->test_labels = malloc(sets->n_test * sizeof(int));
		// printf("%d %d %d\n", sets->n_train, sets->n_train + sets->n_test, n_samples);
		for (i = 0, j = sets->n_train; j < sets->n_train + sets->n_test; i++, j++) { /* Start after the training labels
																					  */
			sets->test_samples[i] = malloc(strlen(labels[j]) + 1);
			strcpy(sets->test_samples[i], labels[j]);

			sets->test_labels[i] = sets->test_samples[i][strlen(sets->test_samples[i]) - 1] - '0';
			sets->test_samples[i][strlen(sets->test_samples[i]) - 1] = '\0';

			free(labels[j]);
#ifdef SHOWPROGRESS
			printf("\r%.1f%%", rescale_lo_hi(cnt++, 0, 2 * (sets->n_train + sets->n_test + sets->n_validate), 0, 100));
			fflush(stdout);
#endif
		}
		shuffle_arrays_similar(sets->test_samples, sets->test_labels, sets->n_test, sizeof(char *), sizeof(int));

		sets->validate_samples = malloc(sets->n_validate * sizeof(char *));
		sets->validate_labels = malloc(sets->n_validate * sizeof(int));
		for (int i = 0, j = sets->n_train + sets->n_test; j < sets->n_train + sets->n_test + sets->n_validate;
			 i++, j++) {
			sets->validate_samples[i] = malloc(strlen(labels[j]) + 1);

			strcpy(sets->validate_samples[i], labels[j]);
			sets->validate_labels[i] = sets->validate_samples[i][strlen(sets->validate_samples[i]) - 1] - '0';
			sets->validate_samples[i][strlen(sets->validate_samples[i]) - 1] = '\0';

			free(labels[j]);
#ifdef SHOWPROGRESS
			printf("\r%.1f%%", rescale_lo_hi(cnt++, 0, 2 * (sets->n_train + sets->n_test + sets->n_validate), 0, 100));
			fflush(stdout);
#endif
		}
		free(labels);
		shuffle_arrays_similar(sets->validate_samples, sets->validate_labels, sets->n_validate, sizeof(char *),
							   sizeof(int));
		return sets;
	}
	return NULL;
}

void construct_train_set(Datasets *sets, char **positive_labels, char **negative_labels)
{
	sets->train_samples = malloc(2 * sets->n_train * sizeof(char *));
	sets->train_labels = malloc(2 * sets->n_train * sizeof(int));
	int i, j;
	for (i = 0, j = 0; i < 2 * sets->n_train; i += 2, j++) {
		sets->train_samples[i] = malloc(strlen(positive_labels[j]) + 1);
		sets->train_samples[i + 1] = malloc(strlen(negative_labels[j]) + 1);

		strcpy(sets->train_samples[i], positive_labels[j]);
		strcpy(sets->train_samples[i + 1], negative_labels[j]);

		sets->train_labels[i] = sets->train_samples[i][strlen(sets->train_samples[i]) - 1] - '0';
		sets->train_samples[i][strlen(sets->train_samples[i]) - 1] = '\0';

		sets->train_labels[i + 1] = sets->train_samples[i + 1][strlen(sets->train_samples[i + 1]) - 1] - '0';
		sets->train_samples[i + 1][strlen(sets->train_samples[i + 1]) - 1] = '\0';
#ifdef SHOWPROGRESS
		printf("\r%.1f%%", rescale_lo_hi(cnt++, 0, 2 * (sets->n_train + sets->n_test + sets->n_validate), 0, 100));
		fflush(stdout);
#endif
	}
	shuffle_arrays_similar(sets->train_samples, sets->train_labels, 2 * sets->n_train, sizeof(char *), sizeof(int));
}

void construct_test_set(Datasets *sets, char **positive_labels, char **negative_labels)
{
	sets->test_samples = malloc(2 * sets->n_test * sizeof(char *));
	sets->test_labels = malloc(2 * sets->n_test * sizeof(int));
	int i, j;
	for (i = 0, j = sets->n_test; i < 2 * sets->n_test; i += 2, j++) { /* Start after the training labels */
		sets->test_samples[i] = malloc(strlen(positive_labels[j]) + 1);
		sets->test_samples[i + 1] = malloc(strlen(negative_labels[j]) + 1);

		strcpy(sets->test_samples[i], positive_labels[j]);
		strcpy(sets->test_samples[i + 1], negative_labels[j]);

		sets->test_labels[i] = sets->test_samples[i][strlen(sets->test_samples[i]) - 1] - '0';
		sets->test_samples[i][strlen(sets->test_samples[i]) - 1] = '\0';

		sets->test_labels[i + 1] = sets->test_samples[i + 1][strlen(sets->test_samples[i + 1]) - 1] - '0';
		sets->test_samples[i + 1][strlen(sets->test_samples[i + 1]) - 1] = '\0';
#ifdef SHOWPROGRESS
		printf("\r%.1f%%", rescale_lo_hi(cnt++, 0, 2 * (sets->n_train + sets->n_test + sets->n_validate), 0, 100));
		fflush(stdout);
#endif
	}
	shuffle_arrays_similar(sets->test_samples, sets->test_labels, 2 * sets->n_test, sizeof(char *), sizeof(int));
}

void construct_validation_set(Datasets *sets, char **positive_labels, char **negative_labels)
{
	sets->validate_samples = malloc(2 * sets->n_validate * sizeof(char *));
	sets->validate_labels = malloc(2 * sets->n_validate * sizeof(int));
	int i, j;
	for (i = 0, j = sets->n_train + sets->n_test; i < 2 * sets->n_validate; i += 2, j++) {
		sets->validate_samples[i] = malloc(strlen(positive_labels[j]) + 1);
		sets->validate_samples[i + 1] = malloc(strlen(negative_labels[j]) + 1);

		strcpy(sets->validate_samples[i], positive_labels[j]);
		strcpy(sets->validate_samples[i + 1], negative_labels[j]);

		sets->validate_labels[i] = sets->validate_samples[i][strlen(sets->validate_samples[i]) - 1] - '0';
		sets->validate_samples[i][strlen(sets->validate_samples[i]) - 1] = '\0';

		sets->validate_labels[i + 1] = sets->validate_samples[i + 1][strlen(sets->validate_samples[i + 1]) - 1] - '0';
		sets->validate_samples[i + 1][strlen(sets->validate_samples[i + 1]) - 1] = '\0';
#ifdef SHOWPROGRESS
		printf("\r%.1f%%", rescale_lo_hi(cnt++, 0, 2 * (sets->n_train + sets->n_test + sets->n_validate), 0, 100));
		fflush(stdout);
#endif
	}
	shuffle_arrays_similar(sets->validate_samples, sets->validate_labels, 2 * sets->n_validate, sizeof(char *),
						   sizeof(int));
}

double sigmoid(double *x, double *w, int n)
{
	double f = w[0];
	for (int i = 1; i < n; ++i) {
		f += x[i - 1] * w[i];
	}
	return ((double) 1) / (1.0 + exp(-f));
}

double loss(double sigmoid, double y)
{
	return y * log(sigmoid) + (1 - y) * log(1 - sigmoid);
}

int predicted_label(double result, double threshold)
{
	return (result > threshold) ? 1 : 0;
}

void parse_relation(char *relation, char **doc1, char **doc2)
{
	// char *		str = NULL, *temp = NULL;
	const char *del = " ,\n";
	char *		context = NULL;
	char *		token = NULL;
	char *		temp_relation = malloc(strlen(relation) + 1); /* Copy because the relation must remain untouched */
	strcpy(temp_relation, relation);
	// printf("temp_relation = %s\n", temp_relation);
	if ((token = strtok_r(temp_relation, del, &context)) == NULL) {
		printf("token1 = %s\n", token);
		// perror("Failed to tokenize string: ");
		exit(EXIT_FAILURE);
	}
	*doc1 = malloc(strlen(token) + 1);
	strcpy(*doc1, token);
	if ((token = strtok_r(NULL, del, &context)) == NULL) {
		printf("token2 = %s\n", token);
		// perror("Failed to tokenize string: ");
		exit(EXIT_FAILURE);
	}
	*doc2 = malloc(strlen(token) + 1);
	strcpy(*doc2, token);
	free(temp_relation);
	// str = malloc(strlen(relation) + 1);
	// char **temp = str;
	// strcpy(str, relation);
	// // str = relation;
	// while (str[0] != ',')
	// 	str = str + 1;

	// str[0] = '\0';

	// *doc1 = malloc(strlen(relation) + 1);
	// strcpy(*doc1, relation);	// we got the first product
	// temp = str + 1;				// get the second product

	// while (str[0] != ',')
	// 	str = str + 1;

	// str[0] = '\0';
	// *doc2 = malloc(strlen(temp) + 1);
	// strcpy(*doc2, temp);
	// free(str);
}
