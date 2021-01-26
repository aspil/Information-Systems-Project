#pragma once
#include "vectorizer.h"

void words_idf_add_value(struct hash_map *, char *, char *);

void compute_tf_values(Vectorizer *);

void compute_idf_values(Vectorizer *);

void compute_tfidf_values(Vectorizer *vectorizer);

void tfidf_reduce_features(Vectorizer *vectorizer);