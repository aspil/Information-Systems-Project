#pragma once

#include "../../include/util/gen_types.h"

struct queue *queue_init(DeleteValueFunction destroy_value);

int queue_size(struct queue *queue);

void *dequeue(struct queue *queue);

void enqueue(struct queue *queue, void *value);

void queue_remove(struct queue *queue);

void queue_delete(struct queue *queue);