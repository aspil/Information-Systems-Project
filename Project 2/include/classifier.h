#pragma once


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
