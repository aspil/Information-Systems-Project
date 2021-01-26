#pragma once
#include <pthread.h>
struct hash_map; /* Forward declaration */

typedef struct Vectorizer {
	int max_features;
	int documents_count;
	int type;

	pthread_mutex_t mutex;

	struct hash_map *features;
	struct hash_map *word_frequencies;
	struct hash_map *words_idf;
} Vectorizer;

int get_stopwords(char *stopwords_file);

Vectorizer *vectorizer_init(int, int);

void vectorizer_delete(Vectorizer *);

void vectorizer_fit_transform(Vectorizer *vectorizer, char *path, int max_features);

void vectorizer_transform(Vectorizer *vectorizer, int max_features);

void vectorizer_fit(Vectorizer *vectorizer, char *path);

double *vectorizer_get_vector(Vectorizer *vectorizer, char *document1, char *document2);

void vectorizer_construct(Vectorizer *, char *);

void word_frequencies_add_value(struct hash_map *, char *, char *);

void vectorizer_reduce_features(Vectorizer *vectorizer, int max_features);

void parse_json(Vectorizer *, char *, char *, char *);