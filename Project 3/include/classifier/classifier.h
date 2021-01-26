#pragma once
#include "classifier_utils.h"

struct vector;

typedef struct Vectorizer Vectorizer;

typedef struct Datasets Datasets;

typedef struct LogisticRegressor {
	double learning_rate;
	int	   epochs;
	int	   batch_size;

	Datasets *		   datasets;
	double *		   weights;
	int				   n_weights;
	struct Vectorizer *vect;
} LogisticRegressor;

LogisticRegressor *Logistic_Regression_Init(double learning_rate, int epochs, int batch_size);

void Logistic_Regression_Delete(LogisticRegressor *model);

void Logistic_Regression_fit(LogisticRegressor *model, Datasets *sets, Vectorizer *vect);

double **create_batch(LogisticRegressor *model, int start, int end);

void train(LogisticRegressor *model, int n_threads);

void mini_batch_gradient_descent(void *args);

void compute_weights(LogisticRegressor *model, int threads);

double *gradient(LogisticRegressor *model, double **batch, int *labels, int start, int end);

int *test(LogisticRegressor *model, int n_threads);

void resolve_transitivity_issues(LogisticRegressor *model, struct vector *v);

int stochastic_gradient_descent(LogisticRegressor *, double *, int);

// int *test(LogisticRegressor *classifier, char **labels, int n_labels);