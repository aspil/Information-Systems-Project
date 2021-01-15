#pragma once
#include "vectorizer.h"

void words_idf_add_value(struct hash_map *, char *, char *);

void compute_tf_values(struct vectorizer *);

void compute_idf_values(struct vectorizer *);

void compute_tfidf_values(struct vectorizer *vectorizer);

void tfidf_reduce_features(struct vectorizer *vectorizer, int max_features);