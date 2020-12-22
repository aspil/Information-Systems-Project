#pragma once
#include "vector.h"
#include "types.h"

struct vector;	/* Redeclare to avoid warnings because of include cycle between vector.h and clique.h */

struct spec {
	char *name;
	char **value;
	int cnt;
};

struct spec* spec_init(char *spec_name, struct vector *vec);
void spec_delete(void *ptr);

struct product {
	int  	   id;
	char *website;
	struct list *specs;	  	/* List of struct product specifications */
	struct product *next; 	/* Pseudo-list of products */
	struct clique *clique;	/* Pointer to the product's clique */
};

struct product* product_init(int id, char *website, struct clique *ptr);

void product_delete(struct product *p);


struct clique {
	int size;
	struct product *first_product;
	struct product *last_product;
};

struct clique * create_clique();

void merge_cliques(struct clique *c1, struct clique *c2);

void delete_clique(void *c);

/* Adds a spec to the list of a clique's first product */
void push_specs(struct clique *ptr, char *spec , struct vector *vec);

int vector_search_clique(struct vector *vec, struct clique *address);

int vector_search_product(struct vector *vec, struct product *address);