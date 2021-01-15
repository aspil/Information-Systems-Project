#pragma once
#include "../../include/clique/clique.h"

#include <stdio.h>

struct hash_map;

void shuffle_string_array(char *array[], int size);

int count_json_files(char *);

int pick_the_buckets_and_features(int argc, char **argv, int *features, int *print);

int arg_parse(int argc, char **argv);

char *extract_spec_title(char *);

char *extract_spec_value(char *, FILE *);

int print_results(struct hash_map *map);

int print_negative_results(struct hash_map *map);

int make_the_files(struct hash_map *map);

void positive_relations_file(char *name_of_file, struct clique *clique_ptr);

void negative_relations_file(char *					   name_of_file,
							 struct clique *		   clique_ptr,
							 struct negative_relation *ptr);

void print_cliques(int print_number);
