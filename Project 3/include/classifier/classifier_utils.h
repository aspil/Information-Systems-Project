#pragma once

typedef struct Datasets {
	char **train_samples;
	int *  train_labels;
	int	   n_train;

	char **test_samples;
	int *  test_labels;
	int	   n_test;

	char **validate_samples;
	int *  validate_labels;
	int	   n_validate;
} Datasets;

typedef struct augmented_training_set {
	char **train_samples;
	int *  train_labels;
	int	   n_train;
} Augmented_training_set;

double *create_weights(int);

int load_labels(char *positive_labels_file, char **positive_labels, char *negative_labels_file, char **negative_labels);

int get_label_count(char *);

Datasets *train_test_split(double train_percent, double test_percent, int stratify);

void construct_train_set(Datasets *sets, char **positive_labels, char **negative_labels);

void construct_test_set(Datasets *sets, char **positive_labels, char **negative_labels);

void construct_validation_set(Datasets *sets, char **positive_labels, char **negative_labels);

double sigmoid(double *x, double *w, int n);

int predicted_label(double result, double threshold);

double loss(double sigmoid, double y);

void parse_relation(char *relation, char **doc1, char **doc2);