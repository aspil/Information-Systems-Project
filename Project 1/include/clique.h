#pragma once

/*
 * Struct: product_values
 * ----------------------
 * It describes a product's spec
 * by a name and its value
 */
struct product_values {
	char *name;
	char **value;
};

struct spec_list
{
	struct product_values spec;
	struct spec_list *next;
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
	struct spec_list *next_spec;	  /* List of struct product_values */
	struct spec_list *last_spec;
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

void product_init(struct clique *ptr,int id, char *website);

void push_specs(struct clique *ptr,char * spec , char **value);