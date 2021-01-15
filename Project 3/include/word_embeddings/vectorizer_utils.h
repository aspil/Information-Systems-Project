#pragma once

#include "../../modules/list/list.h"

struct triple {
	int	   a_i;
	double b_d;
	double c_d;
};

struct vectorizer_elem {
	char *file;
	char *word;
	union {
		int	   frequency;
		double tfidf;
	};
};

struct idf_elem {
	char *		 word;
	double		 idf;
	double		 average_tfidf;
	struct list *files;
};

struct feature {
	char * data;
	double priority;
};

struct vectorizer_elem *vectorizer_elem_init(char *, char *, int);

void delete_vectorizer_elem(void *);

struct idf_elem *idf_elem_init(char *);

void idf_elem_delete(void *);

struct feature *feature_init(char *str, double p);

int compare_features(void *f1, void *f2);

void feature_delete(void *f);