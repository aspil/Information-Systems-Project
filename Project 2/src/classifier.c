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

// void Logistic_Regression_transform() {

// }

void read_labels(char *labels_path, char **labels) {
	size_t len = 0, read;
	char *line = NULL;
	FILE *fp = fopen(labels_path,"r");

	if (fp == NULL)
		printf("Cannot open directory '%s'\n", labels_path);


	int label_count = 0;
	if ((read = getline(&line, &len, fp)) != -1)
	{
		get_line_without_end_line(line);
		if (strcmp(line,"left_spec_id,right_spec_id,label") == 0) //it follows the coding i want
		{
			while ((read = getline(&line, &len, fp)) != -1) {
				get_line_without_end_line(line);
				labels[label_count] = malloc(strlen(line)+1);
				strcpy(labels[label_count], line);
				label_count++;
			}
		}
	}
}

int get_label_count(char *labels_path) {
	size_t len = 0;
	size_t read;
	char *line;
	int n_labels = 0;
	FILE *fp = fopen(labels_path,"r");
	if (fp == NULL)
		printf("Cannot open directory '%s'\n", labels_path);

	if ((read = getline(&line, &len, fp)) != -1)
	{
		get_line_without_end_line(line);
		if (strcmp(line,"left_spec_id,right_spec_id,label") == 0) //it follows the coding i want
		{
			while ((read = getline(&line, &len, fp)) != -1)
				n_labels++;
		}
	}
	return n_labels;
}

void train_test_split(struct LogisticRegressor* model, char *path, double train_percent, double test_percent) {
	int n_labels = get_label_count(path);
	FILE *fptr, *ptr;

	char **labels = malloc(n_labels * sizeof(char*));
	read_labels(path, labels);
	
	model->n_train_labels = (int) ((double) train_percent * n_labels);
	model->n_test_labels = (int) ((double) test_percent * n_labels) + 1;

	shuffle_string_array(labels, n_labels);

	if ((fptr = fopen("Datasets/train_labels.csv", "w")) == NULL)
		perror("Failed:");
	
	for (int i = 0; i < model->n_train_labels; ++i)
	{
		fputs(labels[i], fptr);
		fputs("\n", fptr); //end each line
	}

	fclose(fptr);

	ptr = fopen("Datasets/test_labels.csv", "w");
	if ((ptr = fopen("Datasets/test_labels.csv", "w")) == NULL)
		perror("Failed:");
	
	for (int i = model->n_train_labels; i < n_labels; ++i)
	{
		fputs(labels[i], ptr);
		fputs("\n", ptr); //end each line
	}
	fclose(ptr);
}
void train(struct LogisticRegressor *classifier, char *labels_path) {
	char *line = NULL, *str = NULL, *document1 = NULL, *document2 = NULL, *temp = NULL;
	size_t read, len = 0;
	int label;
	FILE *fptr;
	if ((fptr = fopen("Datasets/train_dataset.csv", "r")) == NULL) {
		perror("train:");
		return;
	}
	// clock_t t;
	// double time_elapsed1, time_elapsed2;
	// double avg_time1 = 0.0, avg_time2 = 0.0;
	while ((read = getline(&line, &len, fptr)) != -1) {
		get_line_without_end_line(line);
		str = line;

		while (str[0] != ',')
			str = str + 1;

		str[0] = '\0';

		document1 = malloc(strlen(line)+1);

		strcpy(document1, line); // we got the first product
		temp = str + 1 ; // get the second product

		while (str[0] != ',')
			str = str + 1;

		str[0] = '\0';
		document2 = malloc(strlen(temp)+1);
		label = line[strlen(line)-1] == '1';
		strcpy(document2,temp);
		// printf("%s %s\n",document1,document2);

		double *x = vectorizer_get_vector(classifier->vect, document1, document2);
		
		stochastic_gradient_descent(classifier, x, label);

		free(x);
		free(document1);
		free(document2);
	}

	// for (int i = 0; i < classifier->n_weights; ++i) {
	//  	printf("%f\n",classifier->weights[i]);
	// }
	// printf("Non zero weights: %d\n",counter);
}