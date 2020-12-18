#pragma once
#include "../include/map.h"
#include "../include/clique.h"

/*
 * Struct: product_values
 * ----------------------
 * Remove the extension from
 * a filename.
 */
void strip_ext(char *filename);

int count_files(char*path);

int pick_the_buckets(int arg_c,char **arg_v);

void construct_product(struct clique **ptr,char *path,char *id, char *site);

void get_line_without_end_line(char *str);

int print_results(struct hash_map *map);

int print_negative_results(struct hash_map *map);
