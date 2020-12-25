#pragma once
#include "vector.h"
#include "types.h"

struct vector;	/* Redeclare to avoid warnings because of include cycle between vector.h and clique.h */
struct hash_map;

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
	struct clique ***clique;	/* Pointer to the product's clique */
};

struct product* product_init(int id, char *website, struct clique **ptr);

void product_set_clique(struct hash_map *map, void *key);

void product_delete(struct product *p, int counter);

struct negative_relation {
	struct clique *neg_rel;
	struct negative_relation *next;
};

struct clique {
	int size;
	struct product *first_product;
	struct product *last_product;
	struct negative_relation *first_negative;
	struct negative_relation *last_negative;
};

struct clique** create_clique();

void clique_set_first_product(struct clique **ptr, char *id, char *site);

void delete_clique(void *c);

void merge_cliques(struct clique **clique1, struct clique **clique2);

void negative_relation_func(struct clique **clique_1, struct clique **clique_2);

void push_specs(struct clique *ptr, char *spec , struct vector *vec);

int vector_search_clique(struct vector *vec, struct clique *address);

int vector_search_product(struct vector *vec, struct product *address);

int search_and_change(char *first_id, char *second_id, struct hash_map *map, int relation);