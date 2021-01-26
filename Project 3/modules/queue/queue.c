#include "queue.h"

#include "../list/list.h"

#include <assert.h>
#include <pthread.h>
#include <stdlib.h>

struct queue {
	pthread_mutex_t mtx;
	struct list *	list;
};

struct queue *queue_init(DeleteValueFunction destroy_value)
{
	struct queue *queue = malloc(sizeof(struct queue));
	queue->list = list_init(NULL, destroy_value);
	pthread_mutex_init(&(queue->mtx), NULL);
	return queue;
}

int queue_size(struct queue *queue)
{
	pthread_mutex_lock(&(queue->mtx));
	int size = list_size(queue->list);
	pthread_mutex_unlock(&(queue->mtx));
	return size;
}

void enqueue(struct queue *queue, void *value)
{
	pthread_mutex_lock(&(queue->mtx));
	list_append(queue->list, value);
	pthread_mutex_unlock(&(queue->mtx));
}

void *dequeue(struct queue *queue)
{
	pthread_mutex_lock(&(queue->mtx));
	void *rv = list_first_value(queue->list);
	list_remove_front(queue->list);
	pthread_mutex_unlock(&(queue->mtx));
	return rv;
}

void queue_delete(struct queue *queue)
{
	list_delete(queue->list);
	free(queue);
}