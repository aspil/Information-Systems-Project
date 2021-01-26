#pragma once

#include <pthread.h>

#define TRAIN_THREAD 0
#define TEST_THREAD	 1

typedef struct LogisticRegressor LogisticRegressor;

typedef void (*Routine)(void*);

typedef struct Job {
	Routine routine;
	void*	args;
} Job;

struct routine_args {
	LogisticRegressor* classifier;
	int				   start;
	int				   end;
	int				   thread_id;
};

typedef struct JobScheduler {
	int			  execution_threads; /* Number of execution threads */
	int			  shared_counter;
	struct queue* jobs; /* A queue that holds submitted jobs / tasks */
	pthread_t*	  tids; /* Thread pool */

	pthread_t scheduler_t;

	pthread_mutex_t thread_cnt_lock;
	pthread_mutex_t weights_lock;
	pthread_mutex_t empty_queue_lock;
	pthread_mutex_t mutex;

	pthread_cond_t empty_queue;
	pthread_cond_t threads_done;
	pthread_cond_t compute_weights;
	pthread_cond_t worker_sync;
	pthread_cond_t new_thread_batch;

} JobScheduler;

struct scheduler_args {
	LogisticRegressor* classifier;
	JobScheduler*	   sch;
};

Job* create_job(Routine routine, LogisticRegressor* model, int start, int end);

JobScheduler* initialize_scheduler(LogisticRegressor* model,
								   int				  execution_threads,
								   int				  type,
								   void* (*worker_function)(void*),
								   void* (*weight_calculator_function)(void*) );
/* Call to add a job to the scheduler */
void submit_job(JobScheduler* sch, Job* j);

/* Start making and submitting jobs in a loop */
int execute_all_jobs(JobScheduler* sch);

void wait_all_tasks_finish(JobScheduler* sch);

void* thread_train_work(void* arg);

void* thread_test_work(void* arg);

void* weights_calculator(void* arg);

void destroy_scheduler(JobScheduler* sch);

void wait_scheduler_weights(JobScheduler* sch);

void wait_test_threads(JobScheduler* sch);