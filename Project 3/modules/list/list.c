#include "../../modules/list/list.h"

#include "../../include/clique/clique.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
struct list *list_init(CompareFunction comp, DeleteValueFunction delete_val)
{
	struct list *l = malloc(sizeof(struct list));
	l->size = 0;
	l->head = NULL;
	l->tail = NULL;
	l->delete_value = delete_val;
	l->compare = comp;
	return l;
}
unsigned int list_size(struct list *l)
{
	return l->size;
}
int list_empty(struct list *l)
{
	return l->size == 0;
}
void list_append(struct list *l, void *item)
{
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

int list_find(struct list *l, void *data)
{
	struct list_node *temp = l->head;
	while (temp != NULL) {
		if (l->compare(temp->data, data) == 0)
			return 1;
		temp = temp->next;
	}
	return 0;
}

void *list_get(struct list *l, int pos)
{
	struct list_node *temp = l->head;
	int				  cnt = pos;
	while (cnt > 0) {
		temp = temp->next;
		cnt--;
	}
	return temp->data;
}
void list_remove_front(struct list *l)
{
	struct list_node *temp = l->head;
	assert(temp != NULL);
	l->head = l->head->next;
	if (l->delete_value != NULL)
		l->delete_value(temp->data);
	free(temp);
	l->size--;
}

void list_delete(struct list *l)
{
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

// void list_print_products(struct list *l)
// {
// 	struct list_node *temp = l->head;
// 	while (temp != NULL) {
// 		printf("%d ", (int) ((struct product *) temp->data)->id);
// 		printf("%s\n", (char *) ((struct product *) temp->data)->website);
// 		temp = temp->next;
// 	}
// 	printf("\n");
// }

void *list_first_value(struct list *l)
{
	return (!list_empty(l)) ? l->head->data : NULL;
}

void *list_last_value(struct list *l)
{
	return (!list_empty(l)) ? l->tail->data : NULL;
}

struct list_node *list_first(struct list *l)
{
	return l->head;
}

struct list_node *list_last(struct list *l)
{
	return l->tail;
}