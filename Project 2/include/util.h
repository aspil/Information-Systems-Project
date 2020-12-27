#pragma once
#include <stdio.h>
#include "../include/clique.h"

struct hash_map;

void shuffle_string_array(char* array[], int size);

void strip_ext(char*);

void get_line_without_end_line(char*);

void skip_whitespace(char*);

int get_stopwords(char *stopwords_file);

int count_files(char*);

char *strrem(char*, const char*);

int pick_the_buckets(int, char**);

struct vectorizer;

void parse_json(struct vectorizer*, char*, char*, char*);

char* preprocess_text(char*);

char* extract_spec_title(char*);

char* extract_spec_value(char*, FILE*);

int print_results(struct hash_map *map);

int print_negative_results(struct hash_map *map);

int make_the_files(struct hash_map *map);

void positive_relations_file(char *name_of_file,struct clique *clique_ptr);

void negative_relations_file(char *name_of_file,struct clique *clique_ptr,struct negative_relation *ptr);