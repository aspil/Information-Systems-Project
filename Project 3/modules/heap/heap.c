#include "heap.h"

#include "../dynamic_array/vector.h"

#include <assert.h>
#include <stdlib.h>

static void swap(struct heapq *heap, int id1, int id2)
{
	void *value1 = vector_get(heap->vector, id1 - 1);
	void *value2 = vector_get(heap->vector, id2 - 1);

	vector_set(heap->vector, id1 - 1, value2);
	vector_set(heap->vector, id2 - 1, value1);
}

static void heapify_up(struct heapq *heap, int id)
{
	if (id == 1)
		return;
	int parent_node = id / 2;

	/* If parent's priority is less than the child's, swap them */
	if (heap->compare(vector_get(heap->vector, parent_node - 1), vector_get(heap->vector, id - 1)) <
		0) {
		swap(heap, parent_node, id);
		heapify_up(heap, parent_node); /* Continue the restoration */
	}
}

static void heapify_down(struct heapq *heap, int id)
{
	int left_child = 2 * id;
	int right_child = left_child + 1;

	int size = heapq_size(heap);
	if (left_child > size)
		return;

	int max = left_child;
	/* Get the larger child amongst the left and right */
	if (right_child <= size && heap->compare(vector_get(heap->vector, left_child - 1),
											 vector_get(heap->vector, right_child - 1)) < 0)
		max = right_child;

	/* If the child's priority is bigger than the parent's, swap the two nodes */
	if (heap->compare(vector_get(heap->vector, id - 1), vector_get(heap->vector, max - 1)) < 0) {
		swap(heap, id, max);
		heapify_down(heap, max);
	}
}

struct heapq *heapq_init(CompareFunction compare, DeleteValueFunction destroy_value)
{
	assert(compare != NULL);

	struct heapq *heap = malloc(sizeof(struct heapq));
	heap->compare = compare;
	heap->delete_value = destroy_value;

	heap->vector = vector_init(0, NULL);

	return heap;
}

int heapq_size(struct heapq *heap)
{
	return vector_size(heap->vector);
}

void *heapq_peek(struct heapq *heap)
{
	return vector_get(heap->vector, 0);
}

void heapq_insert(struct heapq *heap, void *value)
{
	vector_push_back(heap->vector, value);

	/* Heapify up to the node to restore the heap prpperty for the node that was inserted. */
	heapify_up(heap, heapq_size(heap));
}

void heapq_extract(struct heapq *heap)
{
	int last_node = heapq_size(heap);
	assert(last_node != 0);

	swap(heap, 1, last_node);
	vector_remove_last(heap->vector);

	heapify_down(heap, 1);
}

void heapq_delete(struct heapq *heap)
{
	heap->vector->delete_value = heap->delete_value;

	vector_delete(heap->vector);

	free(heap);
}