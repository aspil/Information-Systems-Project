#include <stdlib.h>

#include "../include/vector.h"

struct vector* vector_init(int size = VECTOR_MIN_CAPACITY, DeleteValueFunction delete_val) {
	struct vector *v = malloc(sizeof(struct vector));

	v->size = size;
	v->delete_value = delete_val;

	return v;
}

int vector_get_size(struct vector *v) {
	return v->size;
}