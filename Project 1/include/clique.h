#pragma once

/*
 * Struct: product_values
 * ----------------------
 * It describes a product's spec
 * by a name and its value
 */
struct product_values {
	char *name;
	char *value;
};
/*
 * Struct: product
 * ---------------
 * It describes a product by its id, the website it
 * came from and a list of its own specs
 */
struct product {
	int  	   id;
	char *website;
	// struct list values;	  /* List of struct product_values */
	struct product *next; /* Pseudo-list of products */
};

struct product* create_product(int id, const char *website);


struct clique {
	int size;
	struct product *first_product;
	struct product *last_product;
};

void merge_cliques(struct clique *c1, struct clique *c2);