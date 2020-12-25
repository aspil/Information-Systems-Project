#pragma once

struct hash_map;	/* Forward declaration */

struct vectorizer_elem {
	char *file;
	char *word;
	union {
		int frequency;
		double tfidf;
	};
};

struct vectorizer_elem* vectorizer_elem_init(char*, char*, int);

void delete_vectorizer_elem(void*);

struct list;

struct idf_elem {
	char *word;
	double idf;
	double average_tfidf;
	struct list *files;
};

struct idf_elem* idf_elem_init(char*);

void idf_elem_delete(void*);


struct vectorizer {
	int documents_count;

	struct hash_map *features;
	char **features1;

	int max_features;
	struct hash_map *word_frequencies;
	struct hash_map *words_idf;
};

struct vectorizer* vectorizer_init(int, int);

void vectorizer_delete(struct vectorizer*);

struct feature {
	char *data;
	double priority;
};
struct feature* feature_init(char *str, double p);

void feature_delete(void *f);

void vectorizer_fit_transform(struct vectorizer *vectorizer, char *path, int max_features);

void vectorizer_transform(struct vectorizer *vectorizer, int max_features);

void vectorizer_fit(struct vectorizer *vectorizer, char *path);

double* vectorizer_get_vector(struct vectorizer *vectorizer, char *document1, char *document2);

void vectorizer_construct(struct vectorizer*, char*);

void word_frequencies_add_value(struct hash_map*, char*, char*);

void words_idf_add_value(struct hash_map*, char*, char*);

void compute_tf_values(struct vectorizer*);

void compute_idf_values(struct vectorizer*);

void compute_tfidf_values(struct vectorizer *vectorizer);

void reduce_features(struct vectorizer *vectorizer, int max_features);