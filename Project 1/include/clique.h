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
// struct spec_list
// {
// 	struct product_values spec;
// 	struct spec_list *next;
// };
/*
 * Struct: product
 * ---------------
 * It describes a product by its id, the website it
 * came from and a list of its own specs
 */
struct product {
	int  	   id;
	char *website;
	struct list *specs;	  /* List of struct product_values */
	// struct spec_list *last_spec;
	struct product *next; /* Pseudo-list of products */
	struct clique *clique;
};

struct product* create_product(int id, const char *website);


struct clique {
	int size;
	struct product *first_product;
	struct product *last_product;
};

void merge_cliques(struct clique *c1, struct clique *c2) ;

//void merge_cliques(struct clique *c1, struct clique *c2);

struct clique * create_new();

void delete_clique(void *c);

struct product* product_init(int id, char *website, struct clique *ptr);
void product_delete(struct product *p);
void push_specs(struct clique *ptr, char *spec , struct vector *vec);