#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include "../include/util.h"
#include "../include/classifier.h"
#include "../include/vectorizer.h"

extern double learning_rate;
double previous_loss;

int load_labels(char *positive_labels_file, char **positive_labels, char *negative_labels_file, char **negative_labels) {

	size_t len = 0, read;
	char *line = NULL;
	FILE *fp;

	/* Load the positive labels */
	if ((fp = fopen(positive_labels_file, "r")) == NULL) {
		fprintf(stderr, "Error while opening '%s': ", positive_labels_file);
		return perror(""), -1;
	}

	int label_count = 0;

	while ((read = getline(&line, &len, fp)) != -1) {
		get_line_without_end_line(line);
		positive_labels[label_count] = malloc(strlen(line)+1);
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
		negative_labels[label_count] = malloc(strlen(line)+1);
		strcpy(negative_labels[label_count], line);
		label_count++;
	}
	fclose(fp);
	return 0;
}


struct labels_sets* train_test_split(
	char **positive_labels,
	int n_positive_labels,
	char **negative_labels,
	int n_negative_labels,
	double train_percent,
	double test_percent,
	int stratify)
{
	double validate_percent = 1 - train_percent - test_percent;
	if (validate_percent <= 0) {
		fprintf(stderr, "Validation set percentage is negative.\nPlease make sure that the training and test proportions add up into a value strictly less than 1.\n");
		return NULL;
	}
	if (stratify == 1) {
		/* Get the minimum count among the 0 and 1 labels. */
		int n_samples;		
		
		if (n_positive_labels < n_negative_labels) {	/* Sample the negative labels (0s) */
			n_samples = n_positive_labels;
			/* Shuffle before sampling */
			shuffle_string_array(negative_labels, n_negative_labels);

			/* Remove the excess negative samples */
			for (int i = n_samples; i < n_negative_labels; ++i)
				free(negative_labels[i]);
			
		}
		else if (n_negative_labels < n_positive_labels) {	/* Sample the positive labels (1s) */
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
		sets->n_train_labels += n_samples - sets->n_train_labels - sets->n_test_labels - sets->n_validate_labels;

		int i;	// Counter used for each set (train, test, label)
		int j;	// Counter used to advance to the next label in positive and negative labels arrays

		/* Store alternating positive and negative relations(labels) on all three arrays */

		/* Create the training set array */
		sets->train_set = malloc(2 * sets->n_train_labels * sizeof(char*));

		for (i = 0, j = 0; i < 2 * sets->n_train_labels; i += 2, j++) {
			sets->train_set[i] = malloc(strlen(positive_labels[j])+1);
			strcpy(sets->train_set[i], positive_labels[j]);

			sets->train_set[i+1] = malloc(strlen(negative_labels[j])+1);
			strcpy(sets->train_set[i+1], negative_labels[j]);
		}

		/* Now construct the test set */
		sets->test_set_input = malloc(2 * sets->n_test_labels * sizeof(char*));
		for (i = 0, j = sets->n_train_labels; i < 2 * sets->n_test_labels; i += 2, j++) {	/* Start after the training labels */
			sets->test_set_input[i] = malloc(strlen(positive_labels[j])+1);
			strcpy(sets->test_set_input[i], positive_labels[j]);
			
			sets->test_set_input[i+1] = malloc(strlen(negative_labels[j])+1);
			strcpy(sets->test_set_input[i+1], negative_labels[j]);


		}

		/* Finally construct the validation set */
		sets->validate_set = malloc(2 * sets->n_validate_labels * sizeof(char*));

		for (i = 0, j = sets->n_train_labels + sets->n_test_labels; i < 2 * sets->n_validate_labels; i += 2, j++) {	/* Start after the testing labels */
			sets->validate_set[i] = malloc(strlen(positive_labels[j])+1);
			strcpy(sets->validate_set[i], positive_labels[j]);

			sets->validate_set[i+1] = malloc(strlen(negative_labels[j])+1);
			strcpy(sets->validate_set[i+1], negative_labels[j]);
		}
		/* Shuffle them all one last time */
		shuffle_string_array(sets->train_set, sets->n_train_labels);

		shuffle_string_array(sets->test_set_input, sets->n_test_labels);

		/* Split the test set input pairs from their labels(ground truth) */
		sets->test_set_labels = malloc(2 * sets->n_test_labels * sizeof(int));
		for (int i = 0; i < sets->n_test_labels; ++i) {
			sets->test_set_labels[i] = sets->test_set_input[i][strlen(sets->test_set_input[i])-1] - '0';
			sets->test_set_input[i][strlen(sets->test_set_input[i])-1] = '\0';
		}

		shuffle_string_array(sets->validate_set, sets->n_validate_labels);

		return sets;
	}
	return NULL;
}


double* create_weights(int number_of_variables)
{
	double *array_of_weights = malloc(sizeof(double)*number_of_variables);
	for (int i = 0; i < number_of_variables; ++i)
		array_of_weights[i] = 0;

	return array_of_weights;
}

struct LogisticRegressor* Logistic_Regression_Init() {
	struct LogisticRegressor *model = malloc(sizeof(struct LogisticRegressor));
	return model;
}

void Logistic_Regression_fit(struct LogisticRegressor *model, struct vectorizer *vect) {
	model->vect = vect;
	model->n_weights = 2*vect->max_features + 1;
	model->weights = create_weights(model->n_weights);
}

int get_label_count(char *labels_path) {
	size_t len = 0;
	size_t read;
	char *line = NULL;
	int n_labels = 0;
	FILE *fp;
	
	if ((fp = fopen(labels_path, "r")) == NULL) {
		fprintf(stderr, "Error while opening '%s': ", labels_path);
		return perror(""), -1;
	}

	while ((read = getline(&line, &len, fp)) != -1)
		n_labels++;

	fclose(fp);
	return n_labels;
}


double *min_weights;
double min_loss;
#include "../include/map.h"
void train(struct LogisticRegressor *classifier, char **labels, int n_labels) {
	char *str = NULL, *document1 = NULL, *document2 = NULL, *temp = NULL;

	int label;

	min_weights = malloc(classifier->n_weights * sizeof(double));
	min_loss = 1000;
	// clock_t t;
	// double time_elapsed1, time_elapsed2;
	// double avg_time1 = 0.0, avg_time2 = 0.0;
	for (int i = 0; i < 30000; ++i) {
		str = labels[i];

		while (str[0] != ',')
			str = str + 1;

		str[0] = '\0';

		document1 = malloc(strlen(labels[i])+1);
		strcpy(document1, labels[i]); // we got the first product
		temp = str + 1 ; // get the second product

		while (str[0] != ',')
			str = str + 1;

		str[0] = '\0';
		document2 = malloc(strlen(temp)+1);
		strcpy(document2,temp);
		str++;
		label = str[0]-'0';
		double *x = vectorizer_get_vector(classifier->vect, document1, document2);
		
		if (stochastic_gradient_descent(classifier, x, label) < 0) {
			// printf("Stopping at %d relations\n", i);
			// break;
		}

		free(x);
		free(document1);
		free(document2);
	}
	int counter = 0;
	for (int i = 0; i < classifier->n_weights; ++i) {
		classifier->weights[i] = min_weights[i];
		if (classifier->weights[i] != 0)
			counter++;
	 	// printf("%f\n",classifier->weights[i]);
	}

	printf("min loss = %f\n",min_loss);
	printf("Non zero weights: %d\n",counter);
}



int stochastic_gradient_descent(struct LogisticRegressor *classifier, double *x_vector, int result) {
	double sigmoid_result = 0, f;

	/* Fnd the y from equation and take its sigmoid value */

	f = classifier->weights[0];

	for (int i = 1; i < classifier->n_weights; ++i)
		f += x_vector[i-1] * classifier->weights[i];
	
	/* Now use the sigmoid function */
	sigmoid_result = ((double) 1)/( 1 + exp(f));
	// printf("σ(f) = %f\n",sigmoid_result);
	double loss = -1*result*log(sigmoid_result) - (1-result) * log(1-sigmoid_result);
	// printf("prev L = %d")
	

	for (int i = 1; i < classifier->n_weights; ++i)
		if (x_vector[i-1] != 0)	//estimate the difference for each weight and multiply it with the learning rate
			classifier->weights[i] = classifier->weights[i] - ((sigmoid_result-result) * x_vector[i-1]) * learning_rate;
	
	// printf("loss = %f\n",loss);
	if (loss < min_loss) {
		min_loss = loss;
		for (int i = 1; i < classifier->n_weights; ++i)
			min_weights[i] = classifier->weights[i];
	}
	
	return 0;
}


int* test(struct LogisticRegressor *classifier, char **labels, int n_labels) {
	char *str = NULL, *document1 = NULL, *document2 = NULL, *temp = NULL;

	// clock_t t;
	// double time_elapsed1, time_elapsed2;
	// double avg_time1 = 0.0, avg_time2 = 0.0;
	int *predictions = malloc(n_labels*sizeof(double));
	for (int i = 0; i < n_labels; ++i) {
		str = labels[i];

		while (str[0] != ',')
			str = str + 1;

		str[0] = '\0';

		document1 = malloc(strlen(labels[i])+1);
		strcpy(document1, labels[i]); // we got the first product
		temp = str + 1 ; // get the second product

		while (str[0] != ',')
			str = str + 1;

		str[0] = '\0';
		document2 = malloc(strlen(temp)+1);
		strcpy(document2,temp);
		// str++;
		// label = str[0]-'0';
		double *x = vectorizer_get_vector(classifier->vect, document1, document2);
		
		double sigmoid_result = 0, f;

		/* Fnd the y from equation and take its sigmoid value */
		f = classifier->weights[0];

		for (int i = 1; i < classifier->n_weights; ++i)
			f += x[i-1] * classifier->weights[i];
		
		/* Now use the sigmoid function */
		sigmoid_result = ((double) 1)/( 1 + exp(f));
		predictions[i] = (sigmoid_result > 0.5) ? 1 : 0;

		free(x);
		free(document1);
		free(document2);
	}
	return predictions;
	// printf("%d %d\n",classifier->true_positives+classifier->true_negatives+classifier->false_positives+classifier->false_negatives, n_labels);
}

double accuracy_score(int *y_true, int *y_pred, int n) {
	int tp = 0, tn = 0, fp = 0, fn = 0;
	
	for (int i = 0; i < n; ++i) {
		if (y_true[i] == 1) {
			if (y_pred[i] == 1)
				tp++;
		
			else if (y_pred[i] == 0)
				fn++;
		}
		else if (y_true[i] == 0) {
			if (y_pred[i] == 0)
				tn++;
			
			else if (y_pred[i] == 1)
				fp++;
		}
	}
	return (tp+tn) / (1.0 * (tp+tn+fp+fn));
}

double precision_score(int *y_true, int *y_pred, int n) {
	int tp = 0, tn = 0, fp = 0, fn = 0;
	
	for (int i = 0; i < n; ++i) {
		if (y_true[i] == 1) {
			if (y_pred[i] == 1)
				tp++;
		
			else if (y_pred[i] == 0)
				fn++;
		}
		else if (y_true[i] == 0) {
			if (y_pred[i] == 0)
				tn++;
			
			else if (y_pred[i] == 1)
				fp++;
		}
	}
	return tp / (1.0 * (tp + fp));
}

double recall_score(int *y_true, int *y_pred, int n) {
	int tp = 0, tn = 0, fp = 0, fn = 0;
	
	for (int i = 0; i < n; ++i) {
		if (y_true[i] == 1) {
			if (y_pred[i] == 1)
				tp++;
		
			else if (y_pred[i] == 0)
				fn++;
		}
		else if (y_true[i] == 0) {
			if (y_pred[i] == 0)
				tn++;
			
			else if (y_pred[i] == 1)
				fp++;
		}
	}
	return tp / (1.0 * (tp + fn));
}

double f1_score(int *y_true, int *y_pred, int n) {
	int tp = 0, tn = 0, fp = 0, fn = 0;
	
	for (int i = 0; i < n; ++i) {
		if (y_true[i] == 1) {
			if (y_pred[i] == 1)
				tp++;
		
			else if (y_pred[i] == 0)
				fn++;
		}
		else if (y_true[i] == 0) {
			if (y_pred[i] == 0)
				tn++;
			
			else if (y_pred[i] == 1)
				fp++;
		}
	}
	return 2*(recall_score(y_true, y_pred, n) * precision_score(y_true, y_pred, n)) / (recall_score(y_true, y_pred, n) + precision_score(y_true, y_pred, n));
}