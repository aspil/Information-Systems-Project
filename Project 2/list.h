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
	CompareFunction compare;
};

struct list* list_init(CompareFunction comp, DeleteValueFunction delete_val);

int list_empty(struct list *l);

unsigned int list_size(struct list *l);

void list_append(struct list *l, void *item);

void list_delete(struct list *l);

int list_find(struct list *l, void *data);

void* list_get(struct list *l, int pos);

void* list_first_value(struct list *l);

void* list_last_value(struct list *l);

struct list_node* list_first(struct list *l);

struct list_node* list_last(struct list *l);


void list_print_products(struct list *l);