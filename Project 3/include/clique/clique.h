#pragma once

#include "../../modules/dynamic_array/vector.h"

struct vector;
struct hash_map;

struct product {
	int				 id;
	char *			 website;
	struct product * next;	 /* Pseudo-list of products */
	struct clique ***clique; /* Pointer to the product's clique */
};

struct product *product_init(int id, char *website);

void product_set_clique(struct hash_map *map, void *key);

void product_delete(struct product *p, int counter);

struct negative_relation {
	struct clique *			  neg_rel;
	struct negative_relation *next;
};

struct clique {
	int						  size;
	struct product *		  first_product;
	struct product *		  last_product;
	struct negative_relation *first_negative;
	struct negative_relation *last_negative;
};

struct clique **create_clique();

void clique_set_first_product(struct clique **ptr, char *id, char *site);

void delete_clique(void *c);

void merge_cliques(struct clique **clique1, struct clique **clique2);

void negative_relation_func(struct clique **clique_1, struct clique **clique_2);

int vector_search_clique(struct vector *vec, struct clique *address);

int vector_search_product(struct vector *vec, struct product *address);

int search_and_change(char *first_id, char *second_id, struct hash_map *map, int relation);

struct all_info;

int search_and_change2(char *			first_id,
					   char *			second_id,
					   struct hash_map *map,
					   struct all_info *rr_ptr);