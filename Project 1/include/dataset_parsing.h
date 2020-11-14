#pragma once

#include "map.h"

int read_data_files(struct hash_map *ptr,int size,char *path);

void read_relations(struct hash_map *map,char *path);