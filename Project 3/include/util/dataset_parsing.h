#pragma once

#include "../../modules/hashtable/map.h"

struct all_info;

int read_data_files(struct hash_map *ptr, int size, char *path);

void read_relations(struct hash_map *map, char *path);

void read_relations2(struct hash_map *map, char *path, struct all_info *rr_ptr);