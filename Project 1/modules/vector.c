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
void vector_delete(struct vector *vec) {
	if (vec->delete_value != NULL)
		for (int i = 0; i < vec->size; i++) {
			vec->delete_value(vec->array[i].value);
		}

	free(vec->array);
	free(vec);
	return;
}

int vector_search_clique(struct vector *vec, struct clique *address)
{
	struct clique *ptr;

	for (int i = 0; i < vec->size; ++i)
	{
		ptr = (struct clique *) vec->array[i].value;
		if (address->first_product == ptr->first_product)
			return 1;
	}
	return -1;
}

int vector_search_product(struct vector *vec, struct product *address)
{
	struct product *ptr;

	for (int i = 0; i < vec->size; ++i)
	{
		ptr = (struct product *) vec->array[i].value;
		if (address == ptr)
			return 1;
	}
	return -1;
}