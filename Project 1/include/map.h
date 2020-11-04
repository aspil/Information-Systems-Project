#pragma once
typedef unsigned int (*HashFunction) (void*);
typedef unsigned int (*CompareFunction) (void*);
typedef unsigned int (*DeleteKeyFunction) (void*);
typedef unsigned int (*DeleteValueFunction) (void*);

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