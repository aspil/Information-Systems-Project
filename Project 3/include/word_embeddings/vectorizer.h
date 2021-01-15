#pragma once

struct hash_map; /* Forward declaration */

struct vectorizer {
	struct hash_map *features;
	struct hash_map *word_frequencies;
	struct hash_map *words_idf;
	int				 documents_count;
	int				 max_features;
	int				 type;
};

int get_stopwords(char *stopwords_file);

struct vectorizer *vectorizer_init(int, int);

void vectorizer_delete(struct vectorizer *);

void vectorizer_fit_transform(struct vectorizer *vectorizer, char *path, int max_features);

void vectorizer_transform(struct vectorizer *vectorizer, int max_features);

void vectorizer_fit(struct vectorizer *vectorizer, char *path);

double *vectorizer_get_vector(struct vectorizer *vectorizer, char *document1, char *document2);

void vectorizer_construct(struct vectorizer *, char *);

void word_frequencies_add_value(struct hash_map *, char *, char *);

void vectorizer_reduce_features(struct vectorizer *vectorizer, int max_features);

void parse_json(struct vectorizer *, char *, char *, char *);