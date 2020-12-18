#pragma once
#include "types.h"

struct map_node {
	void *key;
	void *value;
	struct map_node *next;
};

struct hash_map {
	struct map_node **array;
	struct map_node **last_chain_bucket;
	int size;
	int count;
	HashFunction hash;
	CompareFunction compare;;
	DeleteKeyFunction delete_key;
	DeleteValueFunction delete_value;
};

struct hash_map* map_init(
	unsigned int size,
	HashFunction hash,
	CompareFunction comp,
	DeleteKeyFunction delete_key,
	DeleteValueFunction delete_val
);

void map_insert(struct hash_map *map, void *key, void *value);

void map_delete(struct hash_map *map);

void* map_find(struct hash_map *map, void *key);
struct map_node* map_find_node(struct hash_map *map, void *key);
unsigned int hash_int(void *key);
unsigned int hash_str(void *key);

int compare_int(void *a, void *b);
int compare_str(void *a, void *b);
int search_and_change(char *first_id,char *second_id,struct hash_map *map,int relation);

void map_print(struct hash_map *map);