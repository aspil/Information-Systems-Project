#pragma once

#include "../../include/clique/clique.h"
#include "../../include/util/gen_types.h"

#define VECTOR_MIN_CAPACITY 7

struct clique;
struct product;

struct vector_node {
	void *value;
};

struct vector {
	struct vector_node *array;
	int					size;
	int					capacity;
	DeleteValueFunction delete_value;
};

struct vector *vector_init(int size, DeleteValueFunction delete_val);

int vector_size(struct vector *v);

void *vector_get(struct vector *vec, int pos);

void vector_set(struct vector *vec, int pos, void *value);

void vector_push_back(struct vector *vec, void *value);

void vector_remove_last(struct vector *);

void vector_delete(void *v);
