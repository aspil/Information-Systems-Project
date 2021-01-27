#include "../../include/classifier/classifier.h"

#include "../../include/classifier/classifier_utils.h"
#include "../../include/classifier/metrics.h"
#include "../../include/scheduler/scheduler.h"
#include "../../include/util/text_preprocessing.h"
#include "../../include/word_embeddings/vectorizer.h"
#include "../../modules/dynamic_array/vector.h"

#include <errno.h>
#include <fcntl.h>
#include <math.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// extern double learning_rate;
// double	previous_loss;
// double *min_weights;
// double	min_loss;

pthread_mutex_t gradient_mtx;

int *predictions;

double *weights;
double *g_gradients;

int n_batches_left;
int threads_working;
int weightsComputed;
int n_epochs_left;
int finish;

LogisticRegressor *Logistic_Regression_Init(double learning_rate, int epochs, int batch_size)
{
	LogisticRegressor *model = malloc(sizeof(struct LogisticRegressor));
	model->learning_rate = learning_rate;
	model->epochs = epochs;
	model->batch_size = batch_size;
	return model;
}

void Logistic_Regression_Delete(LogisticRegressor *model)
{
	free(model->weights);
	free(model);
}

void Logistic_Regression_fit(LogisticRegressor *model, Datasets *sets, Vectorizer *vect)
{
	model->vect = vect;
	model->datasets = sets;
	model->n_weights = 2 * vect->max_features + 1;
	model->weights = create_weights(model->n_weights);
}

double **create_batch(LogisticRegressor *model, int start, int end)
{
	char *document1 = NULL, *document2 = NULL;

	double **X = malloc((end - start + 1) * sizeof(double *));
	int		 j = 0;
	for (int n = start; n <= end; ++n) {
		parse_relation(model->datasets->train_samples[n], &document1, &document2);
		X[j] = vectorizer_get_vector(model->vect, document1, document2);
		free(document1);
		free(document2);
		j++;
	}
	return X;
}

void mini_batch_gradient_descent(void *args)
{
	struct routine_args *r_args = (struct routine_args *) args;

	LogisticRegressor *model = r_args->classifier;

	int start = r_args->start;
	int end = r_args->end;

	double **X = create_batch(model, start, end);

	double *grad = gradient(model, X, model->datasets->train_labels, start, end);

	/* Now add the total gradient of the batch to the global gradient shared amongst the threads */
	pthread_mutex_lock(&gradient_mtx);
	for (int i = 0; i < model->n_weights; ++i) {
		g_gradients[i] += grad[i];
	}
	pthread_mutex_unlock(&gradient_mtx);

	free(grad);
	for (int i = 0; i < end - start; i++) {
		free(X[i]);
	}
	free(X);
}

double *gradient(LogisticRegressor *model, double **batch, int *labels, int start, int end)
{
	double sigmoid_result, error = 0.0;

	double *grad = malloc(model->n_weights * sizeof(double)); /* Local gradient array */
	int		i, label_cnt;
	int		m = end - start + 1; /* end-start + 1 = batch_size */
	for (i = 0, label_cnt = start; (i < m) && (label_cnt < end + 1); ++label_cnt, ++i) {
		// parse_relation(model->datasets->train_samples[n], &document1, &document2);
		// double *x = vectorizer_get_vector(model->vect, document1, document2);

		sigmoid_result = sigmoid(batch[i], model->weights, model->n_weights); /* Get the sigmoid result */

		// error += loss(sigmoid_result, labels[n]); /* Get the loss */

		/* Calculate the gradient for the n'th sample and add it to the previous gradients of the
		 * batch*/
		grad[0] = sigmoid_result - labels[label_cnt];
		for (int j = 1; j < model->n_weights; ++j) {
			grad[j] += (sigmoid_result - labels[label_cnt]) * batch[i][j - 1];
		}
		// free(x);
	}
	for (int j = 0; j < model->n_weights; ++j) {
		grad[j] /= (double) m;
	}
	return grad;
}

void compute_weights(LogisticRegressor *model, int threads)
{
	for (int i = 0; i < model->n_weights; i++) {
		g_gradients[i] /= (double) threads;
		model->weights[i] -= g_gradients[i] * model->learning_rate;
		g_gradients[i] = 0;
	}
}

void resolve_transitivity_issues(LogisticRegressor *model, struct vector *v)
{
	char * new_sample = NULL;
	int	   n, cnt = 0;
	char **new_train_samples = malloc((model->datasets->n_train + vector_size(v)) * sizeof(char *));
	int *  new_train_labels = malloc((model->datasets->n_train + vector_size(v)) * sizeof(int));
	// for (int i = 0; i < model->datasets->n_train; i++) {
	// 	new_train_samples[i] = model->datasets->train_samples[i];
	// 	new_train_labels[i] = model->datasets->train_labels;
	// }

	memcpy(new_train_samples, model->datasets->train_samples, model->datasets->n_train * sizeof(char *));

	memcpy(new_train_labels, model->datasets->train_labels, model->datasets->n_train * sizeof(int));
	for (int i = 0; i < vector_size(v); i++) {
		new_sample = vector_get(v, i);
		char pair[strlen(new_sample)];
		strncpy(pair, new_sample, strlen(new_sample) - 1);

		for (n = 0; n < model->datasets->n_train; n++) {
			if (strcmp(new_train_samples[n], pair) == 0)
				break;
		}
		/* Simply append the new sample */
		if (n == model->datasets->n_train) {
			new_train_samples[cnt] = new_sample;
			cnt++;
		}
		else { /* Replace the sample once it was already in the training set, with the new label */
			free(new_train_samples[n]);
			new_train_samples[n] = new_sample;
		}
	}
}

void predict_batch(void *args)
{
	struct routine_args *r_args = (struct routine_args *) args;

	LogisticRegressor *model = r_args->classifier;

	int start = r_args->start;
	int end = r_args->end;

	char *document1 = NULL, *document2 = NULL;

	for (int i = start; i < end + 1; ++i) {
		parse_relation(model->datasets->test_samples[i], &document1, &document2);

		double *x = vectorizer_get_vector(model->vect, document1, document2);

		/* Get the predicted label using the probability result */
		double sig = sigmoid(x, model->weights, model->n_weights);
		predictions[i] = predicted_label(sig);

		free(x);
		free(document1);
		free(document2);
	}
}

int *test(LogisticRegressor *model, int n_threads)
{
	printf("[Main]: Initializing the scheduler\n");
	JobScheduler *sch = initialize_scheduler(model, n_threads, thread_test_work, NULL);

	predictions = calloc(model->datasets->n_test, sizeof(double));
	finish = 0;
	int n_pred_batches;

	if (model->batch_size > model->datasets->n_test) {
		model->batch_size = model->datasets->n_test / n_threads;

		model->batch_size += (model->datasets->n_test % n_threads) / n_threads;
	}
	// for (int i = 0; i < model->n_weights; i++)
	// 	printf("%f\n", model->weights[i]);
	// printf("\nn_test = %d, batch_size = %d\n\n", model->datasets->n_test, model->batch_size);

	// if (model->datasets->n_test % model->batch_size == 0) {
	// 	n_pred_batches = model->datasets->n_test / model->batch_size;
	// }

	// else {
	// 	n_pred_batches = model->datasets->n_test / model->batch_size + 1;
	// }

	n_pred_batches = model->datasets->n_test / model->batch_size;
	if (model->datasets->n_test % model->batch_size != 0)
		n_pred_batches++;

	threads_working = n_pred_batches;

	int i, start = 0, end = model->batch_size - 1;
	for (i = 0; i < n_pred_batches; i++) {
		Job *new_job = create_job(predict_batch, model, start, end);

		// printf("[Main]: Submitting new job\n");
		submit_job(sch, new_job); /* Enqueue the job */

		if (end == model->datasets->n_test - 1) { /* Stop if we reached the end of the training set */
			// printf("[Main]: i on break is %d\n", i);
			break;
		}

		if ((end + model->batch_size) <= model->datasets->n_test) {
			start += model->batch_size;
			end += model->batch_size;
		}
		else {
			start += model->batch_size;
			end = model->datasets->n_test - 1;
		}
	}
	// printf("[Main]: Wait for the test threads to finish...\n");
	wait_test_threads(sch);
	// printf("[Main]: Signal all to finish\n");
	finish = 1;
	pthread_cond_signal(&sch->threads_done);
	pthread_cond_broadcast(&sch->empty_queue);
	destroy_scheduler(sch);
	return predictions;
}

void train(LogisticRegressor *model, int n_threads)
{
	printf("Training the model...\n");
	pthread_mutex_init(&gradient_mtx, NULL);

	/* Initialize shared memory variables */
	weights = malloc(sizeof(double) * model->n_weights);
	g_gradients = malloc(sizeof(double) * model->n_weights);
	for (int i = 0; i < model->n_weights; ++i) {
		weights[i] = 0;
		g_gradients[i] = 0;
	}

	n_epochs_left = model->epochs;
	weightsComputed = 0;
	threads_working = 1;
	finish = 0;

	JobScheduler *sch = initialize_scheduler(model, n_threads, thread_train_work, weights_calculator);

	/* Start the epochs */
	// printf("Train size: %d\n", model->datasets->n_train);
	// printf("[Main]: Starting the epochs\n");
	for (int epoch = 0; epoch < model->epochs; epoch++) {
		/* Create jobs until we cover all the dataset */
		// printf("\n[Main]: Epoch %d\n\n", epoch);
		int i, start = 0, end = model->batch_size - 1;
		if (model->datasets->n_train % model->batch_size == 0) {
			n_batches_left = model->datasets->n_train / model->batch_size;
		}
		else {
			n_batches_left = model->datasets->n_train / model->batch_size + 1;
		}
		while (n_batches_left > 0) {
			pthread_mutex_lock(&sch->mutex);
			// printf("[Main]: Calculating threads_working\n");
			if (n_batches_left < n_threads) {
				threads_working = n_batches_left;
			}
			else {
				threads_working = n_threads;
			}

			pthread_mutex_unlock(&sch->mutex);
			for (i = 0; i < n_threads; i++) {
				// printf("[Main]: %d iteration: Creating new job with start %d and end %d \n", i, start, end);
				Job *new_job = create_job(mini_batch_gradient_descent, model, start, end);

				// printf("[Main]: Submitting new job\n");
				submit_job(sch, new_job); /* Enqueue the job */

				if (end == model->datasets->n_train - 1) { /* Stop if we reached the end of the training set */
					// printf("[Main]: i on break is %d\n", i);
					break;
				}

				if ((end + model->batch_size) <= model->datasets->n_train) {
					start += model->batch_size;
					end += model->batch_size;
				}
				else {
					start += model->batch_size;
					end = model->datasets->n_train - 1;
				}
			}
			// printf("[Main]: i is %d, n_batches_left is %d\n", i, n_batches_left);
			if (i == 0)
				n_batches_left -= 1;
			else if (i == n_threads)
				n_batches_left -= i;
			else
				n_batches_left -= i + 1;
			// n_batches_left -= i;
			// printf("[Main]: batches left: %d\n", n_batches_left);
			// printf("[Main]: Waiting for the scheduler session\n");
			wait_scheduler_weights(sch);
			// printf("[Main]: Scheduler finished...\n");
		}
		// printf("[Main]: Computing weights...\n");
		// sleep(1);
		n_epochs_left--;
	}

	// printf("[Main]: End of epochs\n");
	finish = 1;
	// printf("[Main]: Signal all to finish\n");
	pthread_cond_signal(&sch->threads_done);
	pthread_cond_broadcast(&sch->empty_queue);
	destroy_scheduler(sch);

	printf("Saving the model's weights to a file\n");
	FILE *fp;
	if ((fp = fopen("data/model/weights.txt", "w")) == NULL) {
		perror("main: can't open weigts.txt:");
		exit(EXIT_FAILURE);
	}
	fprintf(fp, "%d\n", model->n_weights);
	for (int i = 0; i < model->n_weights; ++i)
		fprintf(fp, "%f\n", model->weights[i]);

	fclose(fp);
	// write the testing labels to a separate file
	if ((fp = fopen("data/test/testing_data.csv", "w")) == NULL) {
		perror("main: can't open weigts.txt:");
		exit(EXIT_FAILURE);
	}
	fprintf(fp, "%d\n", model->datasets->n_test);
	for (int i = 0; i < model->datasets->n_test; ++i)
		fprintf(fp, "%s%d\n", model->datasets->test_samples[i], model->datasets->test_labels[i]);

	fclose(fp);
}

// int stochastic_gradient_descent(LogisticRegressor *model, double *x_vector, int result)
// {
// 	double sigmoid_result = 0, f;

// 	/* Find the y from equation and take its sigmoid value */

// 	f = model->weights[0];

// 	for (int i = 1; i < model->n_weights; ++i)
// 		f += x_vector[i - 1] * model->weights[i];

// 	/* Now use the sigmoid function */
// 	sigmoid_result = ((double) 1) / (1.0 + exp(-f));
// 	double loss = -1 * result * log(sigmoid_result) - (1 - result) * log(1 - sigmoid_result);
// 	printf("%f\n", loss);
// 	model->weights[0] = model->weights[0] - ((sigmoid_result - result)) * model->learning_rate;

// 	for (int i = 1; i < model->n_weights; ++i) {
// 		if (x_vector[i - 1] != 0)	 // estimate the difference for each weight and multiply it with the learning rate
// 			model->weights[i] =
// 				model->weights[i] - ((sigmoid_result - result) * x_vector[i - 1]) * model->learning_rate;
// 	}
// 	if (loss < min_loss) {
// 		min_loss = loss;
// 		for (int i = 0; i < model->n_weights; ++i)
// 			min_weights[i] = model->weights[i];
// 	}

// 	return 0;
// }
