#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
	// printf("Inserting %p %p %d %d\n", key, value, *(int*)key, *(int*)value);
	/* The initial bucket of the hashed key is empty */
	if (map->array[pos] == NULL) {
		/* Allocate space for the first node of the bucket in pos index */
		map->array[pos] = malloc(sizeof(struct map_node));
		map->array[pos]->key = key;
		map->array[pos]->value = value;
		// printf("Inserted %p %p %d %d\n", map->array[pos]->key, map->array[pos]->value, *(int*)map->array[pos]->key, *(int*)map->array[pos]->value);

		map->array[pos]->next = NULL;
		map->last_chain_bucket[pos] = map->array[pos];
	}
	else {
		/* Allocate space for a new node on the next node of the last node */
		struct map_node *new_node = malloc(sizeof(struct map_node));
		new_node->key = key;
		new_node->value = value;
		// printf("Inserted %p %p %d %d\n", new_node->key, new_node->value, *(int*)new_node->key, *(int*)new_node->value);

		new_node->next = NULL;
		/* Chain the new node */
		map->last_chain_bucket[pos]->next = new_node;
		/* Change the new last node pointer */
		map->last_chain_bucket[pos] = new_node;
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
void map_delete(struct hash_map *map) {
	struct map_node *temp, *next_x;
	int counter = 0;
	struct clique *node_l;
	struct vector *vec;
	vec = vector_init(1,NULL);
	for (unsigned int i = 0; i < map->size; ++i) {
		temp = map->array[i];
		while (temp != NULL) {
			
			if (map->delete_value != NULL) {
				if (counter==0) {
					node_l = (struct clique *) temp->value;
					
					vector_push_back(vec, node_l->first_product);
					map->delete_value(temp->value);
					counter++;
				}
				else {
					node_l = (struct clique *) temp->value;

					int result = vector_search_product(vec, node_l->first_product);

					if (result==-1) {
						vector_push_back(vec, node_l->first_product);
						map->delete_value(temp->value);
					}
				}
				free(temp->value);
				if (map->delete_key != NULL)
					map->delete_key(temp->key);// eq free(map->array[i]->key);
				
			}
			next_x = temp->next;
			free(temp);

			temp = next_x;
		}
	}
	vector_delete(vec);
	free(map->last_chain_bucket);
	free(map->array);
	free(map);
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

int search_and_change(char *first_id, char *second_id, struct hash_map *map)
{
	//hash the first product to find it 
	struct clique *c1,*c2;
	unsigned int pos = map->hash(first_id) % map->size;
	unsigned int pos_2 = map->hash(second_id) % map->size;

	//printf("Position is : %d %s \n",pos ,(char*)first_id);

	//printf("Position is : %d %s \n",pos_2 ,(char*)second_id);
	struct map_node *search=map->array[pos];

	struct map_node *search_2=map->array[pos_2];

	//printf("%p %p \n",search,search_2 );

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

					// printf("Print the key : %s\n", (char*)search_2->key);
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
			clique = (struct clique*) temp->value;
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