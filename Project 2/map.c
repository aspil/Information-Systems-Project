#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "../include/map.h"
#include "../include/clique.h"
struct hash_map* map_init(
	unsigned int size,
	HashFunction hash,
	CompareFunction comp,
	DeleteKeyFunction delete_key,
	DeleteValueFunction delete_val
) {
	struct hash_map *map = malloc(sizeof(struct hash_map));
	map->size = size;
	map->count = 0;
	/* Allocate memory for the two arrays */
	map->array = malloc(map->size * sizeof(struct map_node*));
	map->last_chain_bucket = malloc(map->size * sizeof(struct map_node*));
	
	/* Assign the function pointers */
	map->hash = hash;
	map->compare = comp;
	map->delete_key = delete_key;
	map->delete_value = delete_val;
	/* Mark the buckets as NULL, meaning empty */
	for (unsigned int i = 0; i < map->size; ++i) {
		map->array[i] = NULL;
	}
	return map;
}

void map_insert(struct hash_map *map, void *key, void *value) {
	unsigned int pos = map->hash(key) % map->size;
	struct clique *go;

	// printf("Inserting %p %p %d %d\n", key, value, *(int*)key, *(int*)value);
	/* The initial bucket of the hashed key is empty */
	if (map->array[pos] == NULL) {
		/* Allocate space for the first node of the bucket in pos index */
		map->array[pos] = malloc(sizeof(struct map_node));
		map->array[pos]->key = key;
		map->array[pos]->value = value;
		map->array[pos]->next = NULL;
		map->last_chain_bucket[pos] = map->array[pos];
		go=*((struct clique**)map->array[pos]->value);
		go->first_product->clique=(struct clique ***)& (map->array[pos]->value);
	}
	else {
		/* Allocate space for a new node on the next node of the last node */
		struct map_node *new_node = malloc(sizeof(struct map_node));
		new_node->key = key;
		new_node->value = value;
		new_node->next = NULL;
		/* Chain the new node */
		map->last_chain_bucket[pos]->next = new_node;
		/* Change the new last node pointer */
		map->last_chain_bucket[pos] = new_node;
		go=*((struct clique**)new_node->value);
		if ((strcmp(go->first_product->website,"www.pricedekho.com")==0) && (go->first_product->id==1242))
		{
		//p->clique = &clique;
		//printf("kapws deuterh %p fora 8a to deis auto to mhnuma kai 8a deis %p kai %p kai %p  \n",sou,go->first_product->clique,&sou,&(new_node->value) );
		}
		go->first_product->clique=(struct clique ***)& (new_node->value);
	}
}

void* map_find(struct hash_map *map, void *key) {
	struct map_node *temp = map->array[map->hash(key) % map->size];
	while (temp != NULL) {
		if (map->compare(temp->key, key) == 0) {
			return temp->value;
		}
		temp = temp->next;
	}
	return NULL;
}
struct map_node* map_find_node(struct hash_map *map, void *key) {
	struct map_node *temp = map->array[map->hash(key) % map->size];
	while (temp != NULL) {
		if (map->compare(temp->key, key) == 0) {
			return temp;
		}
		temp = temp->next;
	}
	return NULL;
}

int search_and_change(char *first_id, char *second_id, struct hash_map *map,int relation)
{
	//hash the first product to find it 
	struct clique **c1,**c2;
	unsigned int pos = map->hash(first_id) % map->size;
	unsigned int pos_2 = map->hash(second_id) % map->size;

	struct map_node *search = map->array[pos];

	struct map_node *search_2 = map->array[pos_2];

	//printf("%p %p \n",search,search_2 );

	if (search == NULL || search_2 == NULL)
	{	
		printf("No such product hashed\n");
		return -1;
	}
	else
	{
		while (search != NULL)
		{
			if (strcmp((char*)search->key, first_id)!=0)
			{
				//they are not the same , look the next 
				search = search->next;
			}
			else
			{	//you found the node u were looking for 
				c1 = (struct clique**) search->value;
				break;

			}
		}

		if (search == NULL)
		{
			printf("No such product hashed \n");
			return -1;
		}
		else
		{
			while (search_2 != NULL)
			{
				if (strcmp((char*)search_2->key,second_id)!=0)
				{
					//they are not the same , look the next 
					search_2 = search_2->next;
				}
				else
				{	//you found the node u werewww.shopmania.in//1317
					c2 = (struct clique**) search_2->value;
					break;

				}
			}
		}
		if (search_2 == NULL)
		{
			printf("No such product hashed \n");
			return -1;
		}
		// you have both cliques you need to merge
		if (relation==1)
		{
			//printf("ena mhnuma\n");
			merge_cliques(c1,c2);
		}
		else 
		{
			//add_negative_relation
			negative_relation_func(c1,c2);
		}
	}
	return 1;
}

void map_delete(struct hash_map *map) {
	printf("Mpes sthn delete\n");
	struct map_node *temp, *next;
	struct vector *vec;
	vec = vector_init(1, NULL);
	for (unsigned int i = 0; i < map->size; ++i) {
		temp = map->array[i];
		while (temp != NULL) {
			if (map->delete_value != NULL) {
					map->delete_value(temp->value);
			}
			if (map->delete_key != NULL)
				map->delete_key(temp->key);

			next = temp->next;
			free(temp);

			temp = next;
		}
	}
	vector_delete(vec);
	free(map->last_chain_bucket);
	free(map->array);
	free(map);
}

void map_print(struct hash_map *map) {
	struct map_node *temp;
	char *key;
	struct clique *clique;
	struct product *product;
	for (unsigned int i = 0; i < map->size; ++i) {
		temp = map->array[i];
		printf("-- Hashed Position %u --\n", i);
		while (temp != NULL) {
			key = (char*) temp->key;
			printf("%s\n", key);
			clique = *(struct clique**) temp->value;
			product = clique->first_product;
			while (product != NULL) {
				printf("\tproduct: %s - %d\n", product->website, product->id);
				product = product->next;
			}
			printf("\tclique's first product: %s - %d\n", (clique->first_product)->website, (clique->first_product)->id);
			printf("\tclique's last product: %s - %d\n", (clique->last_product)->website, (clique->last_product)->id);
			temp = temp->next;
		}
		printf("\n");
	}
	return;
}

unsigned int hash_int(void *key) {
	return *(int*) key;
}

unsigned int hash_str(void *key) {
	unsigned long hash = 5381;
	for(unsigned int c = 0; c < strlen((char*)key); ++c)
		hash = ((hash << 5) + hash) + (char) ((char*)key)[c];	

	return hash;
}

int compare_int(void *a, void *b) {
	return *(int*)a - *(int*)b;
}

int compare_str(void *a, void *b) {
	return strcmp((char*) a, (char*) b);
}
