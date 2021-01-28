#include "../../include/util/util.h"
#include "../../include/word_embeddings/vectorizer.h"
#include "../../include/word_embeddings/vectorizer_utils.h"
#include "../../modules/hashtable/map.h"
#include "../../modules/heap/heap.h"
#include "../../modules/list/list.h"

#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
unsigned int hash_str(void *);

void compute_tf_values(Vectorizer *vectorizer)
{
	int document_words_count = 0;
#ifdef SHOWPROGRESS
	int cnt = 0;
#endif
	for (void *iter = map_begin(vectorizer->word_frequencies); iter != NULL;
		 iter = map_advance(vectorizer->word_frequencies)) {
		struct vector *vec = (struct vector *) iter;
		document_words_count = vector_size(vec);

		for (int i = 0; i < document_words_count; ++i) {
			struct vectorizer_elem *elem = (struct vectorizer_elem *) vector_get(vec, i);
			/* Calculate the tf value for the current word in the
			 * current file. REMINDER: frequency member variable will become
			 * inactive (see vectorizer.h: struct vectorizer_elem); use only tfidf
			 * to avoid undefined behavior. */
			elem->tfidf = (double) elem->frequency / document_words_count;
		}

#ifdef SHOWPROGRESS
		printf("\r%.1f%%", rescale_lo_hi(cnt++, 0, vectorizer->word_frequencies->total_items, 0, 100));
		fflush(stdout);
#endif
	}
#ifdef SHOWPROGRESS
	printf("%c[2K\rDone\n", 27);
#endif
}

void compute_idf_values(Vectorizer *vectorizer)
{
#ifdef SHOWPROGRESS
	int cnt = 0;
#endif
	for (void *iter = map_begin(vectorizer->words_idf); iter != NULL; iter = map_advance(vectorizer->words_idf)) {
		struct idf_elem *elem = (struct idf_elem *) iter;
		elem->idf = log2((double) vectorizer->documents_count / list_size(elem->files));

#ifdef SHOWPROGRESS
		printf("\r%.1f%%", rescale_lo_hi(cnt++, 0, vectorizer->word_frequencies->total_items, 0, 100));
		fflush(stdout);
#endif
	}
#ifdef SHOWPROGRESS
	printf("%c[2K\rDone\n", 27);
#endif
}

void compute_tfidf_values(Vectorizer *vectorizer)
{
	struct idf_elem *idf_val;
	int				 document_words_count;
	double			 idf;
#ifdef SHOWPROGRESS
	int cnt = 0;
#endif

	for (void *iter = map_begin(vectorizer->word_frequencies); iter != NULL;
		 iter = map_advance(vectorizer->word_frequencies)) {
		struct vector *vec = (struct vector *) iter;
		document_words_count = vector_size(vec);

		for (int i = 0; i < document_words_count; ++i) {
			struct vectorizer_elem *elem = (struct vectorizer_elem *) vector_get(vec, i);

			if ((idf_val = (struct idf_elem *) map_find(vectorizer->words_idf, elem->word)) != NULL) {
				idf = idf_val->idf;
				elem->tfidf *= idf;					   /* Now elem has the tfidf value */
				idf_val->average_tfidf += elem->tfidf; /* Add up to the average
														  tfidf of this word */
			}
		}
#ifdef SHOWPROGRESS
		printf("\r%.1f%%", rescale_lo_hi(cnt++, 0, vectorizer->word_frequencies->total_items, 0, 100));
		fflush(stdout);
#endif
	}
#ifdef SHOWPROGRESS
	printf("%c[2K\rDone\n", 27);
#endif
}

void words_idf_add_value(struct hash_map *words_idf, char *file, char *word)
{
	struct idf_elem *elem = NULL;

	if ((elem = map_find(words_idf, word)) != NULL) {
		/* Only append the file if it hasn't been appended yet */
		if (strcmp(list_last_value(elem->files), file) != 0) {
			char *filename = malloc(strlen(file) + 1);
			strcpy(filename, file);
			list_append(elem->files, filename);
		}
	}
	else {
		char *key = malloc(strlen(word) + 1);
		strcpy(key, word);

		char *filename = malloc(strlen(file) + 1);
		strcpy(filename, file);

		elem = idf_elem_init(key);	  // free will be passed to elem's
									  // list which holds string elements
		list_append(elem->files, filename);

		map_insert(words_idf, key, elem);
	}
}

void tfidf_reduce_features(Vectorizer *vectorizer)
{
	for (void *iter = map_begin(vectorizer->words_idf); iter != NULL; iter = map_advance(vectorizer->words_idf)) {
		struct idf_elem *elem = (struct idf_elem *) iter;
		elem->average_tfidf /= vectorizer->documents_count;
	}

	struct heapq *heap = heapq_init(compare_features, feature_delete); /* free will be used only
																		  on struct feature */

	for (void *iter = map_begin(vectorizer->words_idf); iter != NULL; iter = map_advance(vectorizer->words_idf)) {
		struct feature *feat =
			feature_init(((struct idf_elem *) iter)->word, ((struct idf_elem *) iter)->average_tfidf);
		heapq_insert(heap, feat);
	}

	vectorizer->features = map_init(vectorizer->max_features, hash_str, compare_str, free, free);

	int column = 0;
#ifdef SHOWPROGRESS
	printf("Selecting the best %d features...\n", vectorizer->max_features);
#endif
	for (int i = 0; i < vectorizer->max_features; ++i) {
		struct feature *extracted = heapq_peek(heap);

		char *key = malloc(strlen(extracted->data) + 1);
		strcpy(key, extracted->data);

		struct triple *t = malloc(sizeof(struct triple));
		t->a_i = column;
		t->b_d = 0.0;
		t->c_d = 0.0;
		map_insert(vectorizer->features, key, t);

		heapq_extract(heap);
		free(extracted);
		column++;
#ifdef SHOWPROGRESS
		printf("\r%.1f%%", rescale_lo_hi(i++, 0, vectorizer->max_features, 0, 100));
		fflush(stdout);
#endif
	}
#ifdef SHOWPROGRESS
	printf("%c[2K\rDone\n", 27);
#endif
	heapq_delete(heap);
}