#pragma once

struct labels_sets {
	char **train_set;
	int	   n_train_labels;

	char **test_set_input;
	int *  test_set_labels;
	int	   n_test_labels;

	char **validate_set;
	int	   n_validate_labels;
};

double *create_weights(int);

int load_labels(char * positive_labels_file,
				char **positive_labels,
				char * negative_labels_file,
				char **negative_labels);

int get_label_count(char *);

struct labels_sets *train_test_split(char **positive_labels,
									 int	n_positive_labels,
									 char **negative_labels,
									 int	n_negative_labels,
									 double train_percent,
									 double test_percent,
									 int	stratify);

void construct_train_set(struct labels_sets *sets, char **positive_labels, char **negative_labels);

void construct_test_set(struct labels_sets *sets, char **positive_labels, char **negative_labels);

void construct_validation_set(struct labels_sets *sets,
							  char **			  positive_labels,
							  char **			  negative_labels);