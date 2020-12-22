#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdlib.h>
#include <string.h>
#include  "../include/list.h"
#include "../include/clique.h"


void merge_cliques(struct clique **clique_1, struct clique **clique_2) 
{

	//printf("gamw to xristo mou\n");
	struct product *ptr_2 = (*clique_2)->first_product;

	if ((*clique_1)->first_product == (*clique_2)->first_product)
		return;

	(*clique_1)->last_product->next = (*clique_2)->first_product;
	
	/* Update their sizes */
	(*clique_1)->size += (*clique_2)->size;

	/* Update the last product pointer of the first clique */
	(*clique_1)->last_product = (*clique_2)->last_product;

	if (((*clique_2)->first_negative != NULL) && ((*clique_1)->first_negative == NULL)) 
	{
		(*clique_1)->first_negative = (*clique_2)->first_negative;
		(*clique_1)->last_negative = (*clique_2)->last_negative;

		//you need to change the negative relations that show clique 2 cause it will get freed 

		struct negative_relation *tranverse_1 = (*clique_2)->first_negative, *tranverse_2;


		while (tranverse_1 != NULL)
		{
			tranverse_2 = tranverse_1->neg_rel->first_negative;

			while (tranverse_2 != NULL)
			{
				if (tranverse_2->neg_rel == (*clique_2))
				{
					tranverse_2->neg_rel = (*clique_1);
					break;
				}
				tranverse_2 = tranverse_2->next;
			}

			tranverse_1 = tranverse_1->next;

		}
	}
	else if (((*clique_2)->first_negative != NULL) && ((*clique_1)->first_negative != NULL)) 
	{
		
		struct negative_relation *ngt = (*clique_2)->first_negative, *ngt_2, *found = NULL, *prev_cli, *one_more = NULL;
		int count_clique_1 = 0, count_clique_2 = 0;
		while (ngt != NULL)
		{
			ngt_2 = ngt->neg_rel->first_negative;

			prev_cli = ngt_2;

			while (ngt_2 != NULL)
			{
				if (ngt_2->neg_rel == (*clique_2))
				{
					found = ngt_2;
					one_more = prev_cli;
					count_clique_2 = 1;
					
				}
				if (ngt_2->neg_rel == (*clique_1))
				{
					count_clique_1 = 1;
				}
				prev_cli = ngt_2;
				ngt_2 = ngt_2->next;
			}

			if (count_clique_2 == 1)
			{
				if (count_clique_1 == 1)
				{
					if (found == one_more)
					{
						ngt->neg_rel->first_negative = found->next;
						free(found);
					}
					else if (found == prev_cli)
					{
						ngt->neg_rel->last_negative = one_more;
						one_more->next = found->next;
						free(found);
					}
					else
					{
					one_more->next = found->next;
					free(found);
					}
				}
				else
				{
					found->neg_rel = (*clique_1);
				}
			}

			ngt = ngt->next;
			count_clique_1 = 0, count_clique_2 = 0;

		}
		//u should concatenate the 2 lists of negative relations and delete any double adress

		(*clique_1)->last_negative->next = (*clique_2)->first_negative;

		(*clique_1)->last_negative = (*clique_2)->last_negative;

		struct negative_relation *tranverse_1 = (*clique_1)->first_negative, *tranverse_2, *prev;

		while (tranverse_1 != NULL)
		{
			if (tranverse_1->next != NULL)
			{
				tranverse_2 = tranverse_1->next;
				prev = tranverse_1;

			}
			else
				break;
			while(tranverse_2 != NULL)
			{
				if (tranverse_2->neg_rel == tranverse_1->neg_rel)
				{
					prev->next = tranverse_2->next;
					if (tranverse_2->next == NULL)
					{
						(*clique_1)->last_negative = prev;
					}
					free(tranverse_2);
					break;
				}
				prev = tranverse_2;
				tranverse_2 = tranverse_2->next;

			}

			tranverse_1 = tranverse_1->next;
		}
		//the final result is a list with no doubles
		//change the addresses on the other cliques that pointed to the second one
	}

	free(*clique_2);	/* Call free instead of delete_clique because we want to keep the products */
	free(clique_2);
	
	while (ptr_2  !=  NULL) {
		*(ptr_2->clique) = clique_1;
		ptr_2 = ptr_2->next;
	}
	return;

}


void negative_relation_func(struct clique **clique_1, struct clique **clique_2)
{
	// pass the clique 2 to clique 1 list if doesnt exist
	if ((*clique_1)->last_negative == NULL)
	{
		// first node of negative relation
		(*clique_1)->first_negative = malloc(sizeof(struct negative_relation));
 		(*clique_1)->last_negative = (*clique_1)->first_negative;
		(*clique_1)->last_negative->neg_rel = (*clique_2);
		(*clique_1)->last_negative->next = NULL;
	}
	else	// there is already a negative relation 
	{
		/* Check if the negative relation is already there
		 * for another product and if it isn't, add it */
		struct negative_relation *tranverse = (*clique_1)->first_negative, *prev;
		while (tranverse != NULL)
		{
			if (tranverse->neg_rel == (*clique_2))
			{
				break;
			}
			prev = tranverse;
			tranverse = tranverse->next;
		}

		if (tranverse == NULL)
		{
			prev->next = malloc(sizeof(struct negative_relation));
			prev->next->neg_rel = (*clique_2);
			prev->next->next = NULL;
			(*clique_1)->last_negative = prev->next;
		}
	}
	if ((*clique_2)->last_negative == NULL)
	{
		// first node of negative relation
		(*clique_2)->first_negative = malloc(sizeof(struct negative_relation));
		(*clique_2)->last_negative = (*clique_2)->first_negative;
		(*clique_2)->last_negative->neg_rel = (*clique_1);
		(*clique_2)->last_negative->next = NULL;
	}
	else	 // there is already a negative relation 
	{
		/* Check if the negative relation is already there
		 * for another product and if it isn't, add it */
		struct negative_relation *tranverse = (*clique_2)->first_negative, *prev;
		while (tranverse != NULL)
		{
			if (tranverse->neg_rel == (*clique_1))
				break;
			
			prev = tranverse;
			tranverse = tranverse->next;
		}

		if (tranverse == NULL)
		{
			prev->next = malloc(sizeof(struct negative_relation));
			prev->next->neg_rel = (*clique_1);
			prev->next->next = NULL;
			(*clique_2)->last_negative = prev->next;	
		}
	}
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