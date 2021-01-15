#include "../../include/word_embeddings/vectorizer_utils.h"

#include <stdlib.h>
#include <string.h>

int compare_str(void *, void *);

struct vectorizer_elem *vectorizer_elem_init(char *file, char *word, int f)
{
	struct vectorizer_elem *elem = malloc(sizeof(struct vectorizer_elem));

	elem->file = malloc(strlen(file) + 1);
	strcpy(elem->file, file);

	elem->word = malloc(strlen(word) + 1);
	strcpy(elem->word, word);

	elem->frequency = f;
	return elem;
}

void delete_vectorizer_elem(void *elem)
{
	struct vectorizer_elem *b = (struct vectorizer_elem *) elem;
	free(b->file);
	free(b->word);
	free(b);
}

struct idf_elem *idf_elem_init(char *word)
{
	struct idf_elem *elem = malloc(sizeof(struct idf_elem));
	elem->word = word;
	elem->average_tfidf = 0.0;
	elem->idf = 0.0;
	elem->files = list_init(compare_str, free);

	return elem;
}

void idf_elem_delete(void *elem)
{
	struct idf_elem *e = (struct idf_elem *) elem;
	list_delete(e->files);
	free(e);
}

struct feature *feature_init(char *str, double p)
{
	struct feature *f = malloc(sizeof(struct feature));
	f->data = str;
	// f->data = malloc(sizeof(str));
	// strcpy(f->data, str);
	f->priority = p;
	return f;
}

int compare_features(void *f1, void *f2)
{
	if ((double) ((struct feature *) f1)->priority > (double) ((struct feature *) f2)->priority)
		return 1;
	else
		return -1;
}

void feature_delete(void *f)
{
	struct feature *feat = (struct feature *) f;
	// free(feat->data);
	free(feat);
}
