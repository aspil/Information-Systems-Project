#pragma once
#include "../include/util.h"
struct list_node {
	void *data;
	struct list_node *next;
};

struct list {
	unsigned int size;
	struct list_node *head;
	struct list_node *tail;
	DeleteValueFunction delete_value;
};

struct list* list_create(DeleteValueFunction delete_val);

void list_append(struct list *l, void *item);

void list_delete(struct list *l);

int list_get_size(struct list *l);

void list_print_products(struct list *l);