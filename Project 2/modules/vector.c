#include <stdlib.h>
#include <assert.h>
#include "../include/vector.h"

struct vector* vector_init(int cap, DeleteValueFunction delete_val) {
	struct vector *vec = malloc(sizeof(struct vector));
	vec->size = 0;
	vec->capacity = cap < VECTOR_MIN_CAPACITY ? VECTOR_MIN_CAPACITY : cap;
	vec->array = calloc(vec->capacity, sizeof(struct vector_node));
	
	vec->delete_value = delete_val;
	return vec;
}

int vector_size(struct vector *vec) {
	return vec->size;
}
void* vector_get(struct vector *vec, int pos) {
	assert(pos >= 0 && pos < vec->size);
	return vec->array[pos].value;
}

void vector_set(struct vector *vec, int pos, void *value) {
	assert(pos >= 0 && pos < vec->size);

	if (value != vec->array[pos].value && vec->delete_value != NULL)
		vec->delete_value(vec->array[pos].value);

	vec->array[pos].value = value;
	return;
}

void vector_push_back(struct vector *vec, void *value) {
	if (vec->size == vec->capacity) {
		vec->capacity *= 2;
		vec->array = realloc(vec->array, vec->capacity * sizeof(*vec->array));
	}
	vec->array[vec->size].value = value;
	vec->size++;
	return;
}

void vector_remove_last(struct vector *vec) {
	if (vec->delete_value != NULL)
		vec->delete_value(vec->array[vec->size - 1].value);

	vec->size--;

	if (vec->capacity > vec->size * 4 && vec->capacity > 2*VECTOR_MIN_CAPACITY) {
		vec->capacity /= 2;
		vec->array = realloc(vec->array, vec->capacity * sizeof(*vec->array));
	}
}

void vector_delete(void *v) {
	struct vector *vec = (struct vector *) v;
	if (vec->delete_value != NULL)
		for (int i = 0; i < vec->size; i++)
			vec->delete_value(vec->array[i].value);

	free(vec->array);
	free(vec);
	return;
}