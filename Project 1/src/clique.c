#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/clique.h"
#include <stdlib.h>
#include <string.h>
#include  "../include/list.h"
struct product* create_product(int id, const char *website) {
	struct product *p = malloc(sizeof(struct product));
	p->id = id;
	p->website = malloc(strlen(website) * sizeof(char));
	strcpy(p->website, website);
	p->next = NULL;
	return p;
}

void merge_cliques(struct clique *c1, struct clique *c2) {
	/* Update the last product */

	struct product *ptr = c2->first_product;
	struct product *ptr_1=c1->first_product;

	// printf("Bre sapio %s %d \n",c2->last_product->website,c2->last_product->id );

	if (c1->first_product == c2->first_product)
		return;

	c1->last_product->next = c2->first_product;
	// c1->last_product = c2->last_product;
	/* Update their sizes */
	c1->size += c2->size;
	//c2->size = c1->size;
	// struct product *temp = c1->first_product;
	while (ptr != NULL)
	{
		ptr->clique->first_product = c1->first_product;
		ptr->clique->last_product = c2->last_product;
		ptr->clique->size = c1->size;
		ptr = ptr->next;
	}
	// temp = c1->first_product;
	while (ptr_1 != NULL)
	{
		// ptr_1->clique->first_product = c1->first_product;
		ptr_1->clique->last_product = c2->last_product;
		ptr_1->clique->size = c1->size;
		ptr_1 = ptr_1->next;
	}
	return;
}


struct clique* create_new()
{
	struct clique *c = malloc(sizeof(struct clique));
	c->size = 0;
	c->first_product = NULL;
	c->last_product = NULL;
	return c;
}

void delete_clique(void *ptr) {
	struct clique *c = (struct clique *) ptr;
	struct product *temp = (c->first_product), *next = NULL;
	while (temp != NULL) {
		next = (temp)->next;
		product_delete(temp);
		temp = next;
	}
}

struct spec* spec_init(char *spec_name, struct vector *vec) {
	struct spec *spec = malloc(sizeof(struct spec));
	spec->name = malloc(strlen(spec_name)+1);
	strcpy(spec->name, spec_name);

	spec->cnt = vector_size(vec);
	spec->value = malloc(vector_size(vec) * sizeof(struct spec*));
	for (int i = 0; i < spec->cnt; i++) {
		char *spec_val = (char*) vector_get(vec, i);
		spec->value[i] = malloc(strlen(spec_val) + 1);
		strcpy(spec->value[i], spec_val);
	}
	return spec;
}

void spec_delete(void *ptr) {
	struct spec *spec = (struct spec *) ptr;
	free(spec->name);
	for (int i = 0; i < spec->cnt; i++)
		free(spec->value[i]);
	free(spec->value);
	free(spec);
}

struct product* product_init(int id, char *website, struct clique *ptr)
{
	struct product *p = calloc(1,sizeof(struct product));
	p->id = id;
	p->website = malloc(strlen(website)+1);
	p->clique = ptr;
	strcpy(p->website, website);
	p->specs = list_init(compare_str, spec_delete);
	p->next = NULL;
	return p;
}

void product_delete(struct product *p) {
	free(p->website);
	list_delete(p->specs);
	free(p);
	p = NULL;
}

void push_specs(struct clique *ptr, char *spec_name, struct vector *vec)
{
	struct spec *spec = spec_init(spec_name, vec);

	list_append(ptr->first_product->specs, spec);
	return;
}