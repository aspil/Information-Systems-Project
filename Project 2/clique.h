#pragma once
#include "vector.h"
#include "types.h"

struct vector;	/* Redeclare to avoid warnings because of include cycle between vector.h and clique.h */

struct spec {
	char *name;
	char **value;
	int cnt;
};

struct negative_relation
{
	struct clique *neg_rel;
	struct negative_relation *next;
};

struct spec* spec_init(char *spec_name, struct vector *vec);
void spec_delete(void *ptr);

struct product {
	int  	   id;
	char *website;
	struct list *specs;	  	/* List of struct product specifications */
	struct product *next; 	/* Pseudo-list of products */
	struct clique ***clique;	/* Pointer to the product's clique */
};

struct product* product_init(int id, char *website, struct clique **ptr);

void product_delete(struct product *p,int counter);


struct clique {
	int size;
	struct product *first_product;
	struct product *last_product;
	struct negative_relation *first_negative;
	struct negative_relation *last_negative;
};

struct clique** create_clique();

void merge_cliques(struct clique **c1, struct clique **c2);

void negative_relation_func(struct clique **clique_1, struct clique **clique_2);

void delete_clique(void *c);

/* Adds a spec to the list of a clique's first product */
void push_specs(struct clique *ptr, char *spec , struct vector *vec);