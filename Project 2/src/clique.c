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

int vector_search_clique(struct vector *vec, struct clique *address)
{
	struct clique *ptr;

	for (int i = 0; i < vec->size; ++i)
	{
		ptr = (struct clique *) vec->array[i].value;
		if (address->first_product == ptr->first_product)
			return 1;
	}
	return -1;
}

int vector_search_product(struct vector *vec, struct product *address)
{
	struct product *ptr;

	for (int i = 0; i < vec->size; ++i)
	{
		ptr = (struct product *) vec->array[i].value;
		if (address == ptr)
			return 1;
	}
	return -1;
}

int search_and_change(char *first_id, char *second_id, struct hash_map *map)
{
	//hash the first product to find it 
	struct clique *c1,*c2;
	unsigned int pos = map->hash(first_id) % map->size;
	unsigned int pos_2 = map->hash(second_id) % map->size;

	struct map_node *search=map->array[pos];

	struct map_node *search_2=map->array[pos_2];


	if (search == NULL || search_2==NULL)
	{	
		printf("No such product hashed\n");
		return -1;
	}
	else
	{
		while (search!=NULL)
		{
			if (strcmp((char*)search->key, first_id)!=0)
			{
				//they are not the same , look the next 
				search=search->next;
			}
			else
			{	//you found the node u were looking for 

				c1=(struct clique *)search->value;
				break;

			}
		}

		if (search==NULL)
		{
			printf("No such product hashed \n");
			return -1;
		}
		else
		{
			while (search_2!=NULL)
			{
				if (strcmp((char*)search_2->key,second_id)!=0)
				{
					//they are not the same , look the next 
					search_2=search_2->next;
				}
				else
				{	//you found the node u were looking for 
					c2=(struct clique *)search_2->value;
					break;

				}
			}
		}
		if (search_2==NULL)
		{
			printf("No such product hashed \n");
			return -1;
		}
		// you have both cliques you need to merge 
		merge_cliques(c1,c2);
	}
	return 1;
}