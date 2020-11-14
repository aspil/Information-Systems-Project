#pragma once

#include "../include/clique.h"
#include "../include/util.h"

#define VECTOR_MIN_CAPACITY 7

struct vector_node {
	void *value;
};

// Ενα Vector είναι pointer σε αυτό το struct
struct vector {
	struct vector_node *array;
	int size;
	int capacity;
	DeleteValueFunction delete_value;
};

struct vector* vector_init(int size, DeleteValueFunction delete_val);

int vector_get_size(struct vector *v);
void insertArray(Array *a, char *temp);

void initArray_2(Array *a, int initialSize,struct clique *temp);

void insertArray_2(Array *a, struct clique *temp) ;