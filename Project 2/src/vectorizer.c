#include "../include/vectorizer.h"
#include "../include/heap.h"
#include "../include/list.h"
#include "../include/map.h"
#include "../include/util.h"
#include "../include/types.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <ctype.h>
#include <math.h>

struct idf_elem* idf_elem_init(char *word) {
	struct idf_elem* elem = malloc(sizeof(struct idf_elem));
	elem->word = word;
	elem->average_tfidf = 0.0;
	elem->idf = 0.0;
	elem->files = list_init(compare_str, free);
	
	return elem;
}

void idf_elem_delete(void *elem) {
	struct idf_elem* e = (struct idf_elem*) elem;
	list_delete(e->files);
	free(e);
}

struct vectorizer_elem* vectorizer_elem_init(char *file, char *word, int f) {
	struct vectorizer_elem *elem = malloc(sizeof(struct vectorizer_elem));
			
	elem->file = malloc(strlen(file)+1);
	strcpy(elem->file,file);

	elem->word = malloc(strlen(word)+1);
	strcpy(elem->word,word);

	elem->frequency = f;
	return elem;
}

void delete_vectorizer_elem(void *elem) {
	struct vectorizer_elem *b = (struct vectorizer_elem*) elem;
	free(b->file);
	free(b->word);
	free(b);
}

struct vectorizer* vectorizer_init(int files_count, int type) {
	struct vectorizer *v = malloc(sizeof(struct vectorizer));
	v->documents_count = files_count;
	v->word_frequencies = map_init(files_count, hash_str, compare_str, free, vector_delete);
	if (type != 0)
		v->words_idf = map_init(5*files_count, hash_str, compare_str, free, idf_elem_delete);
	
	else v->words_idf = NULL;

	return v;
}

void vectorizer_delete(struct vectorizer* v) {
	map_delete(v->word_frequencies);

	// map_delete(v->features);
	// for (int i = 0; i < v->max_features; ++i)
	// 	free(v->features[i]);
	// free(v->features);

	if (v->words_idf != NULL)
		map_delete(v->words_idf);
	
	free(v);
}