#pragma once

int load_labels(char *positive_labels_file, char **positive_labels, char *negative_labels_file, char **negative_labels);

struct labels_sets* train_test_split(
	char **positive_labels,
	int n_positive_labels,
	char **negative_labels,
	int n_negative_labels,
	double train_percent,
	double test_percent,
	int stratify);


double* create_weights(int);

struct labels_sets {
	char **train_set;
    int n_train_labels;

    char **test_set_input;
    int *test_set_labels;
    int n_test_labels;

    char **validate_set;
    int n_validate_labels;
};

struct LogisticRegressor {
    double *weights;
    int n_weights;
    struct vectorizer *vect;
};

struct LogisticRegressor* Logistic_Regression_Init();

void Logistic_Regression_fit(struct LogisticRegressor*,struct vectorizer*);

void read_labels(char*,char**);

int get_label_count(char*);

void train(struct LogisticRegressor *classifier, char **labels, int n_labels);

int stochastic_gradient_descent(struct LogisticRegressor*,double*,int);

int* test(struct LogisticRegressor *classifier, char **labels, int n_labels);

double accuracy_score(int *y_true, int *y_pred, int n);

double precision_score(int *y_true, int *y_pred, int n);

double recall_score(int *y_true, int *y_pred, int n);

double f1_score(int *y_true, int *y_pred, int n);

