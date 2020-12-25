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
	map->total_items = 0;
	/* Allocate memory for the two arrays */
	map->array = malloc(map->size * sizeof(struct map_node*));
	map->last_chain_bucket = malloc(map->size * sizeof(struct map_node*));
	
	map->iterator = NULL;
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
		map->array[pos]->next = NULL;
		map->last_chain_bucket[pos] = map->array[pos];
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
	}
	map->total_items++;
}

void* map_get_last_inserted_node(struct hash_map *map, void *key) {
	unsigned int pos = map->hash(key) % map->size;
	return map->last_chain_bucket[pos];
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

void* map_begin(struct hash_map *map) {
	int i;

	for (i = 0; i < map->size; ++i) {
		if (map->array[i] != NULL)
			break;
	}
	if (i == map->size)
		return NULL;

	if (i != map->size - 1) {
		if (map->array[i]->next != NULL)
			map->iterator = map->array[i]->next;
		else {
			int j;
			for (j = i+1; i < map->size; ++j) {
				if (map->array[j] != NULL)
					break;
			}
			if (j == map->size)
				return NULL;
			map->iterator = map->array[j];
		}
		// map->next = (map->array[i]->next != NULL) ? map->array[i]->next: map->array[i+1];
	}
	else {
		map->iterator = map->array[i]->next;	// No need to check for null, it's already NULL.
	}
	return map->array[i]->value;
}

void* map_advance(struct hash_map *map) {
	/** If the condition is true, it means the user called the function
	 * after looping through every value in the hash table.
	 * One should use map_begin to reinitialize this pointer to the first element of the map.*/
	// assert(map->next != NULL);
	struct map_node *temp;
	if (map->iterator == NULL)
		return NULL;
	
	unsigned int pos = map->hash(map->iterator->key) % map->size;
	if (pos != map->size-1) {
		temp = map->iterator;
		// map->iterator = (map->iterator != map->last_chain_bucket[pos]) ? temp->next : map->array[pos+1];
		if (map->iterator != map->last_chain_bucket[pos]) {
			map->iterator = temp->next;
		}
		else {
			int i;
			for (i = pos+1; i < map->size; ++i) {
				if (map->array[i] != NULL)
					break;
			}
			if (i == map->size)
				return NULL;
			map->iterator = map->array[i];
		}
		return temp->value;
	}
	else {
		temp = map->iterator;
		map->iterator = (map->iterator != map->last_chain_bucket[pos]) ? temp->next : NULL;
		return temp->value;
	}
}

void map_delete(struct hash_map *map) {
	struct map_node *temp, *next;
	for (unsigned int i = 0; i < map->size; ++i) {
		temp = map->array[i];
		while (temp != NULL) {
			if (map->delete_value != NULL)
					map->delete_value(temp->value);
			
			if (map->delete_key != NULL)
				map->delete_key(temp->key);
			
			next = temp->next;
			free(temp);
			temp = next;
		}
	}
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

/* For debugging purposes only */
// void map_print(struct hash_map *map) {
// 	struct map_node *temp;
// 	char *key;
// 	struct clique *clique;
// 	struct product *product;
// 	for (unsigned int i = 0; i < map->size; ++i) {
// 		temp = map->array[i];
// 		printf("-- Hashed Position %u --\n", i);
// 		while (temp != NULL) {
// 			key = (char*) temp->key;
// 			printf("%s\n", key);
// 			clique = (struct clique*) temp->value;
// 			product = clique->first_product;
// 			while (product != NULL) {
// 				printf("\tproduct: %s - %d\n", product->website, product->id);
// 				product = product->next;
// 			}
// 			printf("\tclique's first product: %s - %d\n", (clique->first_product)->website, (clique->first_product)->id);
// 			printf("\tclique's last product: %s - %d\n", (clique->last_product)->website, (clique->last_product)->id);
// 			temp = temp->next;
// 		}
// 		printf("\n");
// 	}
// 	return;
// }