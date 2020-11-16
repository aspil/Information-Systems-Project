#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../include/list.h"
#include "../include/clique.h"

struct list* list_create(DeleteValueFunction delete_val) {
	struct list *l = malloc(sizeof(struct list));
	l->size = 0;
	l->head = NULL;
	l->tail = NULL;
	l->delete_value = delete_val;
	return l;
}

void list_append(struct list *l, void *item) {
	struct list_node *new_node = malloc(sizeof(struct list_node));
	new_node->data = item;
	new_node->next = NULL;
	if (l->size == 0) {	
		l->head = new_node;
		l->tail = new_node;
	}
	else {
		l->tail->next = new_node;
		l->tail = new_node;
	}
	l->size++;
	return;
}

void list_delete(struct list *l) {
	struct list_node *temp = l->head, *next;
	if (l->size != 0) {
		while (temp != NULL) {
			next = temp->next;
			if (l->delete_value != NULL)
				l->delete_value(temp->data);
			free(temp);
			temp = next;
		}
	}
	free(l);
}

void list_print_products(struct list *l) {
	struct list_node *temp = l->head;
	while (temp != NULL) {
		printf("%d ",(int)((struct product*)temp->data)->id);
		printf("%s\n",(char*)((struct product*)temp->data)->website);
		temp = temp->next;
	}
	printf("\n");
}