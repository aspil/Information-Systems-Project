#pragma once
#include "vector.h"
#include "types.h"

struct heapq {
	struct vector *vector;
	CompareFunction compare;
	DeleteValueFunction delete_value;
};

struct heapq* heapq_init(CompareFunction compare, DeleteValueFunction destroy_value);

int heapq_size(struct heapq *heap);

void* heapq_peek(struct heapq *heap);

void heapq_insert(struct heapq *heap, void *value);

void heapq_extract(struct heapq *heap);

void heapq_delete(struct heapq *heap);