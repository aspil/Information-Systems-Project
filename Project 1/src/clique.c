#include "../include/clique.h"
#include <stdlib.h>
#include <string.h>

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
	c1->last_product->next = c2->first_product;
	c1->last_product = c2->last_product;
	/* Now both cliques refer to the same product sequence */
	c2->first_product = c1->first_product;
	/* Update their sizes */
	c1->size += c2->size;
	c2->size = c1->size;
	
	return;
}