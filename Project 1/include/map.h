#pragma once
typedef unsigned int (*HashFunction) (void*);
typedef int (*CompareFunction) (void*, void*);
typedef void (*DeleteKeyFunction) (void*);
typedef void (*DeleteValueFunction) (void*);

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

void map_clear(struct hash_map *map);

unsigned int hash_int(void *key);
unsigned int hash_str(void *key);