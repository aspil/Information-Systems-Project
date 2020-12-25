#pragma once

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
