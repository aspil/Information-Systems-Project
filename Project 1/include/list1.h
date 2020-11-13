#pragma once

struct list_node {
	void *data;
	struct list_node *next;
};

struct list {
	unsigned int size;
	struct list_node *head;
	struct list_node *tail;

};

struct list* list_create();

void list_append(struct list *l, void *item);

void list_clear(struct list *l);

void list_print_products(struct list *l);