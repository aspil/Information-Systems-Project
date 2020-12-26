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