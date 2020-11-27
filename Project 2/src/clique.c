#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdlib.h>
#include <string.h>
#include  "../include/list.h"
#include "../include/clique.h"


void merge_cliques(struct clique *clique_1, struct clique *clique_2) {
	struct product *ptr_2 = clique_2->first_product;
	struct product *ptr_1 = clique_1->first_product;

	if (clique_1->first_product == clique_2->first_product)
		return;

	clique_1->last_product->next = clique_2->first_product;
	
	/* Update their sizes */
	clique_1->size += clique_2->size;

	/* For every product of the two cliques, visit their clique
	 * and change its first and last product pointers
	 */
	while (ptr_2 != NULL)
	{
		ptr_2->clique->first_product = clique_1->first_product;
		ptr_2->clique->last_product = clique_2->last_product;
		ptr_2->clique->size = clique_1->size;
		ptr_2 = ptr_2->next;
	}
	while (ptr_1 != NULL)
	{
		ptr_1->clique->last_product = clique_2->last_product;
		ptr_1->clique->size = clique_1->size;
		ptr_1 = ptr_1->next;
	}
	return;
}


struct clique* create_clique()
{
	struct clique *c = malloc(sizeof(struct clique));
	c->size = 0;
	c->first_product = NULL;
	c->last_product = NULL;
	return c;
}

void delete_clique(void *ptr) {
	struct clique *clique = (struct clique *) ptr;
	struct product *temp = (clique->first_product), *next = NULL;
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
	for (int i = 0; i < spec->cnt; i++)
		free(spec->value[i]);

	free(spec->value);
	free(spec->name);
	free(spec);
}

struct product* product_init(int id, char *website, struct clique *clique)
{
	struct product *p = calloc(1,sizeof(struct product));
	p->id = id;
	p->website = malloc(strlen(website)+1);
	p->clique = clique;
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

void push_specs(struct clique *clique, char *spec_name, struct vector *vec)
{
	struct spec *spec = spec_init(spec_name, vec);

	list_append(clique->first_product->specs, spec);
	return;
}