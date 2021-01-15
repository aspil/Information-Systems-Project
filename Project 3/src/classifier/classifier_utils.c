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
	double *array_of_weights = malloc(sizeof(double) * number_of_variables);
	for (int i = 0; i < number_of_variables; ++i)
		array_of_weights[i] = 0;

	return array_of_weights;
}

int load_labels(char * positive_labels_file,
				char **positive_labels,
				char * negative_labels_file,
				char **negative_labels)
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
	}
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

struct labels_sets *train_test_split(char **positive_labels,
									 int	n_positive_labels,
									 char **negative_labels,
									 int	n_negative_labels,
									 double train_percent,
									 double test_percent,
									 int	stratify)
{
	double validate_percent = 1 - train_percent - test_percent;
	if (validate_percent <= 0) {
		fprintf(stderr,
				"Validation set percentage is negative.\nPlease make sure that the training and "
				"test proportions add up into a value strictly less than 1.\n");
		exit(EXIT_FAILURE);
	}
	if (stratify == 1) {
		/* Get the minimum count among the 0 and 1 labels. */
		int n_samples;

		if (n_positive_labels < n_negative_labels) { /* Sample the negative labels (0s) */
			n_samples = n_positive_labels;
			/* Shuffle before sampling */
			shuffle_string_array(negative_labels, n_negative_labels);

			/* Remove the excess negative samples */
			for (int i = n_samples; i < n_negative_labels; ++i)
				free(negative_labels[i]);
		}
		else { /* (n_negative_labels <= n_positive_labels) Sample the positive labels (1s) */
			n_samples = n_negative_labels;
			shuffle_string_array(positive_labels, n_positive_labels);

			/* Remove the excess negative samples */
			for (int i = n_samples; i < n_positive_labels; ++i)
				free(positive_labels[i]);
		}
		/* Shuffle again before splitting */
		shuffle_string_array(positive_labels, n_samples);
		shuffle_string_array(negative_labels, n_samples);

		struct labels_sets *sets = malloc(sizeof(struct labels_sets));
		sets->n_train_labels = train_percent * n_samples;
		sets->n_test_labels = test_percent * n_samples;
		sets->n_validate_labels = validate_percent * n_samples;

		/* Add the remaining labels to the training set */
		sets->n_train_labels +=
			n_samples - sets->n_train_labels - sets->n_test_labels - sets->n_validate_labels;

		/* Store alternating positive and negative relations(labels) on all three arrays */

		/* Create the training set array */
		construct_train_set(sets, positive_labels, negative_labels);

		/* Now construct the test set */
		construct_test_set(sets, positive_labels, negative_labels);

		/* Finally construct the validation set */
		construct_validation_set(sets, positive_labels, negative_labels);

		/* Shuffle them all one last time */
		shuffle_string_array(sets->train_set, sets->n_train_labels);

		shuffle_string_array(sets->test_set_input, sets->n_test_labels);

		shuffle_string_array(sets->validate_set, sets->n_validate_labels);

		/* Split the test set input pairs from their labels(ground truth) */
		sets->test_set_labels = malloc(2 * sets->n_test_labels * sizeof(int));
		for (int i = 0; i < 2 * sets->n_test_labels; ++i) {
			sets->test_set_labels[i] =
				sets->test_set_input[i][strlen(sets->test_set_input[i]) - 1] - '0';
			sets->test_set_input[i][strlen(sets->test_set_input[i]) - 1] = '\0';
		}

		sets->n_train_labels *= 2;
		sets->n_test_labels *= 2;
		sets->n_validate_labels *= 2;

		for (int i = 0; i < n_samples; ++i) {
			free(positive_labels[i]);
			free(negative_labels[i]);
		}
		free(positive_labels);
		free(negative_labels);
		return sets;
	}
	return NULL;
}

void construct_train_set(struct labels_sets *sets, char **positive_labels, char **negative_labels)
{
	sets->train_set = malloc(2 * sets->n_train_labels * sizeof(char *));
	int i, j;
	for (i = 0, j = 0; i < 2 * sets->n_train_labels; i += 2, j++) {
		sets->train_set[i] = malloc(strlen(positive_labels[j]) + 1);
		strcpy(sets->train_set[i], positive_labels[j]);

		sets->train_set[i + 1] = malloc(strlen(negative_labels[j]) + 1);
		strcpy(sets->train_set[i + 1], negative_labels[j]);
	}
}

void construct_test_set(struct labels_sets *sets, char **positive_labels, char **negative_labels)
{
	sets->test_set_input = malloc(2 * sets->n_test_labels * sizeof(char *));
	int i, j;
	for (i = 0, j = sets->n_train_labels; i < 2 * sets->n_test_labels;
		 i += 2, j++) { /* Start after the training labels */
		sets->test_set_input[i] = malloc(strlen(positive_labels[j]) + 1);
		strcpy(sets->test_set_input[i], positive_labels[j]);

		sets->test_set_input[i + 1] = malloc(strlen(negative_labels[j]) + 1);
		strcpy(sets->test_set_input[i + 1], negative_labels[j]);
	}
}

void construct_validation_set(struct labels_sets *sets,
							  char **			  positive_labels,
							  char **			  negative_labels)
{
	sets->validate_set = malloc(2 * sets->n_validate_labels * sizeof(char *));
	int i, j;
	for (i = 0, j = sets->n_train_labels + sets->n_test_labels; i < 2 * sets->n_validate_labels;
		 i += 2, j++) { /* Start after the testing labels */
		sets->validate_set[i] = malloc(strlen(positive_labels[j]) + 1);
		strcpy(sets->validate_set[i], positive_labels[j]);

		sets->validate_set[i + 1] = malloc(strlen(negative_labels[j]) + 1);
		strcpy(sets->validate_set[i + 1], negative_labels[j]);
	}
}