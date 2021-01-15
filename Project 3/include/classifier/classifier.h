#pragma once

struct LogisticRegressor {
	double *		   weights;
	int				   n_weights;
	struct vectorizer *vect;
};

struct LogisticRegressor *Logistic_Regression_Init();

void Logistic_Regression_Delete(struct LogisticRegressor *model);

void Logistic_Regression_fit(struct LogisticRegressor *, struct vectorizer *);

void train(struct LogisticRegressor *classifier, char **labels, int n_labels);

int stochastic_gradient_descent(struct LogisticRegressor *, double *, int);

int *test(struct LogisticRegressor *classifier, char **labels, int n_labels);