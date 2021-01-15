#include "queue.h"

#include "../list/list.h"

#include <assert.h>
#include <stdlib.h>

struct queue {
	struct list *list;
};

struct queue *queue_init(DeleteValueFunction destroy_value)
{
	struct queue *queue = malloc(sizeof(struct queue));
	queue->list = list_init(NULL, destroy_value);
	return queue;
}

int queue_size(struct queue *queue)
{
	return list_size(queue->list);
}

void *dequeue(struct queue *queue)
{
	void *rv = list_first_value(queue->list);
	list_remove_front(queue->list);
	return rv;
}

void enqueue(struct queue *queue, void *value)
{
	list_append(queue->list, value);
}

void queue_delete(struct queue *queue)
{
	list_delete(queue->list);
	free(queue);
}