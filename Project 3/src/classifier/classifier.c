#include "../../include/classifier/classifier.h"

#include "../../include/classifier/classifier_utils.h"
#include "../../include/classifier/metrics.h"
#include "../../include/util/text_preprocessing.h"
#include "../../include/word_embeddings/vectorizer.h"

#include <errno.h>
#include <fcntl.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

extern double learning_rate;
double		  previous_loss;
double *	  min_weights;
double		  min_loss;

struct LogisticRegressor *Logistic_Regression_Init()
{
	struct LogisticRegressor *model = malloc(sizeof(struct LogisticRegressor));
	return model;
}

void Logistic_Regression_Delete(struct LogisticRegressor *model)
{
	free(model->weights);
	free(model);
}

void Logistic_Regression_fit(struct LogisticRegressor *model, struct vectorizer *vect)
{
	model->vect = vect;
	model->n_weights = 2 * vect->max_features + 1;
	model->weights = create_weights(model->n_weights);
}

void train(struct LogisticRegressor *classifier, char **labels, int n_labels)
{
	char *str = NULL, *document1 = NULL, *document2 = NULL, *temp = NULL;

	int label;
	min_weights = malloc(classifier->n_weights * sizeof(double));
	min_loss = 1000;
	for (int i = 0; i < n_labels; ++i) {
		str = labels[i];

		while (str[0] != ',')
			str = str + 1;

		str[0] = '\0';

		document1 = malloc(strlen(labels[i]) + 1);
		strcpy(document1, labels[i]);	 // we got the first product
		temp = str + 1;					 // get the second product

		while (str[0] != ',')
			str = str + 1;

		str[0] = '\0';
		document2 = malloc(strlen(temp) + 1);
		strcpy(document2, temp);
		str++;
		label = str[0] - '0';
		double *x = vectorizer_get_vector(classifier->vect, document1, document2);

		stochastic_gradient_descent(classifier, x, label);

		free(x);
		free(document1);
		free(document2);
	}
	int counter = 0;
	for (int i = 0; i < classifier->n_weights; ++i) {
		classifier->weights[i] = min_weights[i];
		if (classifier->weights[i] != 0)
			counter++;
	}
	free(min_weights);
}

int stochastic_gradient_descent(struct LogisticRegressor *classifier, double *x_vector, int result)
{
	double sigmoid_result = 0, f;

	/* Find the y from equation and take its sigmoid value */

	f = classifier->weights[0];

	for (int i = 1; i < classifier->n_weights; ++i)
		f += x_vector[i - 1] * classifier->weights[i];

	/* Now use the sigmoid function */
	sigmoid_result = ((double) 1) / (1.0 + exp(-f));
	double loss = -1 * result * log(sigmoid_result) - (1 - result) * log(1 - sigmoid_result);
	printf("%f\n", loss);
	classifier->weights[0] = classifier->weights[0] - ((sigmoid_result - result)) * learning_rate;

	for (int i = 1; i < classifier->n_weights; ++i) {
		if (x_vector[i - 1] !=
			0)	  // estimate the difference for each weight and multiply it with the learning rate
			classifier->weights[i] = classifier->weights[i] -
									 ((sigmoid_result - result) * x_vector[i - 1]) * learning_rate;
	}
	if (loss < min_loss) {
		min_loss = loss;
		for (int i = 0; i < classifier->n_weights; ++i)
			min_weights[i] = classifier->weights[i];
	}

	return 0;
}

// int mini_batch_gradient_descend()
// {
// 	loss = 0.0;
// 	// for (i = 0; i < batch_size; ++i) {
// 	vectorizer_get_vector();
// 	f = classifier->weights[0];

// 	for (int i = 1; i < classifier->n_weights; ++i)
// 		f += x_vector[i - 1] * classifier->weights[i];
// 	sigmoid_result = ((double) 1) / (1.0 + exp(-f));
// 	loss += -1 * result * log(sigmoid_result) - (1 - result) * log(1 - sigmoid_result);

// 		grad = (sigmoid_result - result) * x_vector[i - 1])
// 	// }
// 	// loss /= -1 * m;
// }

int *test(struct LogisticRegressor *classifier, char **labels, int n_labels)
{
	char *str = NULL, *document1 = NULL, *document2 = NULL, *temp = NULL;

	int *predictions = malloc(n_labels * sizeof(double));

	for (int i = 0; i < n_labels; ++i) {
		str = labels[i];

		while (str[0] != ',')
			str = str + 1;

		str[0] = '\0';

		document1 = malloc(strlen(labels[i]) + 1);
		strcpy(document1, labels[i]);	 // we got the first product
		temp = str + 1;					 // get the second product

		while (str[0] != ',')
			str = str + 1;

		str[0] = '\0';
		document2 = malloc(strlen(temp) + 1);
		strcpy(document2, temp);
		// str++;
		// label = str[0]-'0';
		double *x = vectorizer_get_vector(classifier->vect, document1, document2);

		double sigmoid_result = 0, f;

		/* Fnd the y from equation and take its sigmoid value */
		f = classifier->weights[0];

		for (int i = 1; i < classifier->n_weights; ++i)
			f += x[i - 1] * classifier->weights[i];

		/* Now use the sigmoid function */
		sigmoid_result = ((double) 1) / (1 + exp(-f));

		predictions[i] = (sigmoid_result > 0.5) ? 1 : 0;
		free(x);
		free(document1);
		free(document2);
	}
	return predictions;
}
