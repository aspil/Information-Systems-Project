#include "../../include/scheduler/scheduler.h"

#include "../../include/classifier/classifier.h"
#include "../../modules/queue/queue.h"

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void* thread_work(void* arg);

extern double* g_gradients;
extern double* weights;

extern int n_batches_left;
extern int threads_working;
extern int weightsComputed;
extern int n_epochs_left;
extern int finish;

int threads_finished;

typedef struct Info {
	int counter;
	int type;

	JobScheduler* scheduler;
} Info;

Job* create_job(Routine routine, LogisticRegressor* model, int start, int end)
{
	Job* new_job = malloc(sizeof(struct Job));

	new_job->routine = routine;

	struct routine_args* args = malloc(sizeof(struct routine_args));
	args->classifier = model;
	args->start = start;
	args->end = end;

	new_job->args = (void*) args;

	return new_job;
}

JobScheduler* initialize_scheduler(LogisticRegressor* model,
								   int				  execution_threads,
								   int				  type,
								   void* (*worker_function)(void*),
								   void* (*weight_calculator_function)(void*) )
{
	JobScheduler* scheduler = malloc(sizeof(struct JobScheduler));
	scheduler->execution_threads = execution_threads;
	scheduler->jobs = queue_init(NULL);

	pthread_mutex_init(&(scheduler->thread_cnt_lock), NULL);
	pthread_mutex_init(&(scheduler->empty_queue_lock), NULL);
	pthread_mutex_init(&(scheduler->weights_lock), NULL);
	pthread_mutex_init(&(scheduler->mutex), NULL);

	pthread_cond_init(&(scheduler->empty_queue), NULL);
	pthread_cond_init(&(scheduler->threads_done), NULL);
	pthread_cond_init(&(scheduler->compute_weights), NULL);
	pthread_cond_init(&(scheduler->worker_sync), NULL);
	pthread_cond_init(&(scheduler->new_thread_batch), NULL);

	// pthread_attr_init(&scheduler->attr);
	// pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
	struct scheduler_args* sch_args = malloc(sizeof(struct scheduler_args));
	sch_args->classifier = model;
	sch_args->sch = scheduler;
	if (weight_calculator_function != NULL)
		pthread_create(&(scheduler->scheduler_t), NULL, weights_calculator, (void*) sch_args);

	Info* struct_for_working_thread = NULL;
	scheduler->tids = malloc(execution_threads * sizeof(pthread_t));
	for (int i = 0; i < execution_threads; i++) {
		struct_for_working_thread = malloc(sizeof(struct Info));
		struct_for_working_thread->scheduler = scheduler;
		struct_for_working_thread->counter = i;
		struct_for_working_thread->type = type;
		pthread_create(&scheduler->tids[i], NULL, worker_function, (void*) struct_for_working_thread);

		// pthread_detach(scheduler->tids[i]);
	}
	threads_finished = 0;
	return scheduler;
}

void* thread_train_work(void* arg)
{
	Info* thread_info = (Info*) arg;
	printf("[Thread %d]: Starting\n", thread_info->counter);

	JobScheduler* sch = thread_info->scheduler;
	int			  thread_type = thread_info->type;
	/* Loop as long as there are epochs left to iterate */
	while ((n_epochs_left > 0)) {
		/* Block thread if the job queue is empty */
		pthread_mutex_lock(&sch->empty_queue_lock);
		// printf("[Thread %d]: Waiting on empty queue\n", thread_info->counter);
		/* Thread will also block when the scheduler computes the weights, because
		 * main won't enqueue more jobs before that happens */
		while ((finish == 0) && (queue_size(sch->jobs) == 0)) {
			pthread_cond_wait(&sch->empty_queue, &sch->empty_queue_lock);
		}
		// if (finish == 1) {
		// 	printf("[Thread %d]: mpikaaaaaaaaaaaaaaa\n", thread_info->counter);
		// 	break;
		// }

		pthread_mutex_unlock(&sch->empty_queue_lock);

		// printf("[Thread %d]: Getting job from queue\n", thread_info->counter);
		Job* job = dequeue(sch->jobs);

		void (*func_ptr)(void*);
		void* args_ptr;

		if (job != NULL) {
			func_ptr = job->routine;
			args_ptr = job->args;
			((struct routine_args*) args_ptr)->thread_id = thread_info->counter;

			// printf("[Thread %d]: Executing job\n", thread_info->counter);
			func_ptr(args_ptr); /* Call mini_batch_gradient_descent */
			printf("[Thread %d]: Finished job\n", thread_info->counter);

			free(job->args);
			free(job);

			pthread_mutex_lock(&sch->thread_cnt_lock);
			threads_working--;
			threads_finished++;
			/* The last thread in the current thread group must wake up the scheduler thread */
			if (threads_working == 0) {
				// threads_to_run = sch->threads_working;
				// printf("[Thread %d]: Signal the scheduler\n", thread_info->counter);
				pthread_cond_signal(&sch->threads_done);
			}
			pthread_mutex_unlock(&sch->thread_cnt_lock);
		}
		else {
			// printf("[Thread %d]: Didn't get a job\n", thread_info->counter);
		}
	}
	// printf("[Thread %d]: Exiting...\n", thread_info->counter);
	pthread_exit(NULL);
}

void* thread_test_work(void* arg)
{
	Info* thread_info = (Info*) arg;
	printf("[Thread %d]: Starting\n", thread_info->counter);

	JobScheduler* sch = thread_info->scheduler;
	int			  thread_type = thread_info->type;
	/* Loop as long as there are epochs left to iterate */
	while (finish == 0) {	 // TODO find a safe condition
		/* Block thread if the job queue is empty */
		pthread_mutex_lock(&sch->empty_queue_lock);
		printf("[Thread %d]: Waiting on empty queue\n", thread_info->counter);
		/* Thread will also block when the scheduler computes the weights, because
		 * main won't enqueue more jobs before that happens */
		while ((finish == 0) && (queue_size(sch->jobs) == 0)) {
			pthread_cond_wait(&sch->empty_queue, &sch->empty_queue_lock);
		}
		// if (finish == 1) {
		// 	printf("[Thread %d]: mpikaaaaaaaaaaaaaaa\n", thread_info->counter);
		// 	break;
		// }

		pthread_mutex_unlock(&sch->empty_queue_lock);

		printf("[Thread %d]: Getting job from queue\n", thread_info->counter);
		Job* job = dequeue(sch->jobs);

		void (*func_ptr)(void*);
		void* args_ptr;

		if (job != NULL) {
			func_ptr = job->routine;
			args_ptr = job->args;
			((struct routine_args*) args_ptr)->thread_id = thread_info->counter;

			printf("[Thread %d]: Executing job\n", thread_info->counter);
			func_ptr(args_ptr); /* Call mini_batch_gradient_descent */
			printf("[Thread %d]: Finished job\n", thread_info->counter);

			free(job->args);
			free(job);

			pthread_mutex_lock(&sch->thread_cnt_lock);
			threads_working--;
			threads_finished++;
			/* The last thread in the current thread group must wake up the scheduler thread */
			if (threads_working == 0) {
				// threads_to_run = sch->threads_working;
				printf("[Thread %d]: Signal the scheduler\n", thread_info->counter);
				weightsComputed = 1;
				pthread_cond_signal(&sch->compute_weights);
			}
			pthread_mutex_unlock(&sch->thread_cnt_lock);
		}
		else {
			printf("[Thread %d]: Didn't get a job\n", thread_info->counter);
		}
	}
	printf("[Thread %d]: Exiting...\n", thread_info->counter);
	pthread_exit(NULL);
}

void* weights_calculator(void* arg)
{
	printf("[Scheduler]: Starting\n");
	struct scheduler_args* args = (struct scheduler_args*) arg;

	JobScheduler*	   sch = args->sch;
	LogisticRegressor* model = args->classifier;

	while (n_epochs_left > 0) {
		printf("[Scheduler]: Waiting for the current batch set to finish\n");
		pthread_mutex_lock(&sch->thread_cnt_lock);
		/* Wait for the threads to compute their gradient */
		while ((finish == 0) && (threads_working > 0)) {
			pthread_cond_wait(&sch->threads_done, &sch->thread_cnt_lock);
		}
		if (finish == 1)
			break;
		pthread_mutex_unlock(&sch->thread_cnt_lock);
		printf("[Scheduler]: Calculating the weights\n");

		compute_weights(model, threads_finished);
		printf("threads_finished = %d\n", threads_finished);
		threads_finished = 0;
		printf("[Scheduler]: Finished calculating the weights\n");

		weightsComputed = 1;
		printf("[Scheduler]: Try to wake up the main\n");
		pthread_cond_signal(&sch->compute_weights); /* Signal the main thread to create more jobs */
		threads_working = 1; /* This value is temporary, to block the scheduler on next iteration. It will change from
								main to the correct value */
	}

	printf("[Scheduler]: Exiting...\n");
	pthread_exit(NULL);
}

/* Call to add a job to the scheduler */
void submit_job(JobScheduler* sch, Job* j)
{
	enqueue(sch->jobs, j);
	pthread_cond_signal(&sch->empty_queue);
	// pthread_cond_signal(&sch->threads_done);
}

void wait_scheduler_weights(JobScheduler* sch)
{
	pthread_mutex_lock(&sch->weights_lock);
	while (weightsComputed == 0) {
		pthread_cond_wait(&sch->compute_weights, &sch->weights_lock);
	}
	weightsComputed = 0;
	pthread_mutex_unlock(&sch->weights_lock);
}
void wait_test_threads(JobScheduler* sch)
{
	int i = 0;
	pthread_mutex_lock(&sch->weights_lock);
	while (weightsComputed == 0) {
		printf("[Main]: waiting for %d'th time\n", i);
		pthread_cond_wait(&sch->compute_weights, &sch->weights_lock);
		printf("[Main]: weightsComputed = %d\n", weightsComputed);
		i++;
	}
	// weightsComputed = 0;
	pthread_mutex_unlock(&sch->weights_lock);
}

void wait_all_tasks_finish(JobScheduler* sch)
{
	int	  rc;
	void* status;
	for (int i = 0; i < sch->execution_threads; i++) {
		printf("[Main]: Waiting for thread %d to finish...\n", i);
		if ((rc = pthread_join(sch->tids[i], NULL)) < 0) {
			printf("ERROR; return code from pthread_join() is %d\n", rc);
			exit(EXIT_FAILURE);
		}
		// printf("Main: completed join with thread %ld having a status of %ld\n", i, (long) status);
	}
	printf("[Main]: Waiting for scheduler to finish...\n");
	if ((rc = pthread_join(sch->scheduler_t, NULL)) < 0) {
		printf("ERROR; return code from pthread_join() is %d\n", rc);
		exit(EXIT_FAILURE);
	}
}

void destroy_scheduler(JobScheduler* sch)
{
	wait_all_tasks_finish(sch);

	queue_delete(sch->jobs);
	free(sch->tids);

	pthread_mutex_destroy(&sch->thread_cnt_lock);
	pthread_mutex_destroy(&sch->weights_lock);
	pthread_mutex_destroy(&sch->empty_queue_lock);
	pthread_mutex_destroy(&sch->mutex);

	pthread_cond_destroy(&sch->empty_queue);
	pthread_cond_destroy(&sch->threads_done);
	pthread_cond_destroy(&sch->compute_weights);
	pthread_cond_destroy(&sch->worker_sync);
	pthread_cond_destroy(&sch->new_thread_batch);

	free(sch);
}
