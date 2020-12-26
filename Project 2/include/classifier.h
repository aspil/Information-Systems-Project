#pragma once

double* create_weights(int);

struct LogisticRegressor {
    double *weights;
    int n_weights;
    struct vectorizer *vect;
    int n_train_labels;
    char **train_labels;
    int n_test_labels;
    char **test_labels;
};

struct LogisticRegressor* Logistic_Regression_Init();

void Logistic_Regression_fit(struct LogisticRegressor*,struct vectorizer*);

void Logistic_Regression_transform();


void read_labels(char*,char**);

int get_label_count(char*);

void train_test_split(struct LogisticRegressor*,char*,double,double);

void train(struct LogisticRegressor*,char*);