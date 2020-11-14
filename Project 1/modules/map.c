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
	/* The initial bucket of the hashed key is empty */
	if (map->array[pos] == NULL) {
		/* Allocate space for the first node of the bucket in pos index */
		map->array[pos] = malloc(sizeof(struct map_node));
		map->array[pos]->key = key;
		map->array[pos]->value = value;
		map->array[pos]->next = NULL;
		map->last_chain_bucket[pos] = map->array[pos];
		/* printf("Inserted key %s, and value with size %d, to position %u\n", (char*)map->array[pos]->key,\
		((struct clique*) map->array[pos]->value)->size, pos);*/
	}
	else {
		/* Allocate space for a new node on the next node of the last node */
		struct map_node *new_node = malloc(sizeof(struct map_node*));
		new_node->key = key;
		new_node->value = value;
		new_node->next = NULL;
		/* Chain the new node */
		map->last_chain_bucket[pos]->next = new_node;
		/* Change the new last node pointer */
		map->last_chain_bucket[pos] = new_node;
	}
}

int map_find(struct hash_map *map, void *key, void **value) {
	struct map_node *temp = map->array[map->hash(key) % map->size];
	while (temp != NULL) {
		if (map->compare(key, temp->key) == 0) {
			*value = temp->value;
			return 1;
		}
		temp = temp->next;
	}
	return 0;
}

void map_clear(struct hash_map *map) {
	for (unsigned int i = 0; i < map->size; ++i) {
		if (map->array[i] != NULL) {
			if (map->delete_key != NULL)
				map->delete_key(map->array[i]->key);
			if (map->delete_value != NULL) {
				map->delete_value(map->array[i]->value);
			}
			free(map->array[i]);
		}
	}
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