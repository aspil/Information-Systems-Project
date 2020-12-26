#pragma once
#include <stdio.h>
#include "../include/clique.h"

struct hash_map;

void strip_ext(char*);

void get_line_without_end_line(char*);

void skip_whitespace(char*);

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