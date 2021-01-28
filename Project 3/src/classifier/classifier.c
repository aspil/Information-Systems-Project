#include "../../include/classifier/classifier.h"

#include "../../include/classifier/classifier_utils.h"
#include "../../include/classifier/metrics.h"
#include "../../include/scheduler/scheduler.h"
#include "../../include/util/text_preprocessing.h"
#include "../../include/word_embeddings/vectorizer.h"
#include "../../modules/dynamic_array/vector.h"
#include "../../modules/hashtable/map.h"

#include <errno.h>
#include <fcntl.h>
#include <math.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// extern double learning_rate;
pthread_mutex_t min_loss_mtx;
double			previous_loss;
extern double * min_weights;
extern double	min_loss;

pthread_mutex_t gradient_mtx;
double *		g_gradients;
double *		weights;

int *predictions;

int n_batches_left;
int threads_working;
int weightsComputed;
int n_epochs_left;
int finish;

LogisticRegressor *Logistic_Regression_Init(double learning_rate, int epochs, int batch_size, double predict_threshold)
{
	LogisticRegressor *model = malloc(sizeof(struct LogisticRegressor));
	model->learning_rate = learning_rate;
	model->epochs = epochs;
	model->batch_size = batch_size;
	model->predict_threshold = predict_threshold;
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
		// printf("%f\n", sigmoid_result);
		error += loss(sigmoid_result, labels[i]); /* Get the loss */
		// printf("sig: %f, error: %f\n", sigmoid_result, error);

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
	error /= (double) (-1.0 * m);
	pthread_mutex_lock(&min_loss_mtx);
	if (error < min_loss) {
		min_loss = error;
		for (int i = 0; i < model->n_weights; ++i) {
			min_weights[i] = model->weights[i];
		}
	}
	pthread_mutex_unlock(&min_loss_mtx);
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

void resolve_transitivity_issues(struct vector *v, struct hash_map *map)
{
	struct vector_node *array = v->array;

	char *document1, *document2;

	int i;

	for (i = 0; i < v->size; ++i) {
		// for each relation we are gonna find the clique and change it
		// take the id

		parse_relation((char *) array[i].value, &document1, &document2);

		char *x = (char *) (array[i].value);
		int	  relation = x[strlen(x) - 1] - '0';
		//
		struct clique **c1, **c2;

		unsigned int pos = map->hash(document1) % map->size;
		unsigned int pos_2 = map->hash(document2) % map->size;

		struct map_node *search = map->array[pos];

		struct map_node *search_2 = map->array[pos_2];

		if (search == NULL || search_2 == NULL) {
			printf("\n");
		}
		else {
			while (search != NULL) {
				if (strcmp((char *) search->key, document1) != 0)
					search = search->next;

				else {	  // you found the node u were looking for
					c1 = (struct clique **) search->value;
					break;
				}
			}

			if (search == NULL) {
				printf("\n");
			}

			else {
				while (search_2 != NULL) {
					if (strcmp((char *) search_2->key, document2) != 0)
						search_2 = search_2->next; /* they are not the same , look the next */

					else {
						c2 = (struct clique **) search_2->value;
						break;
					}
				}
			}
			if (search_2 == NULL) {
				printf("\n");
			}
		}

		// i will first check if they are already related with another relation
		int flag = 0;
		if (relation == 1) {
			// go in the negative relations of c1 and search whether c2 exists
			// if it exists throw the relation cause the relation that is there was with higher probability

			struct negative_relation *iteration = (*c1)->first_negative;

			while (iteration != NULL) {
				if (iteration->neg_rel == *c2) {
					// they are related with negative relation
					flag = 1;
					break;
				}
				iteration = iteration->next;
			}

			if ((flag == 0) && ((*c1) != (*c2))) {
				// c1 must take c2 items

				struct product *iteration_first = (*c1)->first_product;

				struct product *iteration_second = (*c2)->first_product;

				while (iteration_first != NULL) {
					while (iteration_second != NULL) {
						char *pair = malloc(strlen(iteration_first->website) + 2 * sizeof(int) +
											strlen(iteration_second->website) + 15);

						sprintf(pair, "%s//%d,%s//%d,%d", iteration_first->website, iteration_first->id,
								iteration_second->website, iteration_second->id, 1);

						// vector_push_back(v, pair);
						iteration_second = iteration_second->next;
					}

					iteration_first = iteration_first->next;
					iteration_second = (*c2)->first_product;
				}

				merge_cliques(c1, c2);
			}
		}
		else {
			if ((*c1) == (*c2)) {
				flag = 1;
			}
			negative_relation_func(c1, c2);
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
		// printf("%f\n", sig);
		predictions[i] = predicted_label(sig, model->predict_threshold);
		// printf("%d\n", predictions[i]);
		free(x);
		free(document1);
		free(document2);
	}
}

int *predict(LogisticRegressor *model, int n_threads)
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
	pthread_mutex_init(&min_loss_mtx, NULL);
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
	clock_t start = clock();
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
	clock_t end = clock();
	printf("Total training time: %f\n", (double) (end - start) / CLOCKS_PER_SEC);
	printf("[Main]: End of epochs\n");
	finish = 1;
	// printf("[Main]: Signal all to finish\n");
	pthread_cond_signal(&sch->threads_done);
	pthread_cond_broadcast(&sch->empty_queue);
	destroy_scheduler(sch);

	for (int i = 0; i < model->n_weights; ++i) {
		// printf("min: %f , model: %f\n", min_weights[i], model->weights[i]);
		model->weights[i] = min_weights[i];
	}
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
