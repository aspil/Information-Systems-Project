#pragma once
#include "../include/map.h"
#include "../include/clique.h"
#include "../include/array.h"


/*
 * Struct: product_values
 * ----------------------
 * Remove the extension from
 * a filename.
 */
void strip_ext(char *filename);
int count_files(char*path);
int pick_the_buckets(int arg_c,char **arg_v);
int read_data_X(struct hash_map *ptr,int size,char *path);
void hash_the_node(struct clique *ptr,char *path,char *id, char *site);
void dataset_y(struct hash_map *map,char *path);
void get_line_without_end_line(char *str);
void print_results(struct hash_map *map);
int compare_array(Array dyn_arr,struct clique *address);