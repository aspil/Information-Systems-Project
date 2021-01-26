#include "../../include/word_embeddings/vectorizer.h"

#include "../../include/util/text_preprocessing.h"
#include "../../include/util/util.h"
#include "../../include/word_embeddings/tfidf.h"
#include "../../include/word_embeddings/vectorizer_utils.h"
#include "../../modules/hashtable/map.h"

#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TFIDF 1
#define BOW	  0

extern struct hash_map *stopwords;

int get_stopwords(char *stopwords_file)
{
	FILE *fp;
	if ((fp = fopen(stopwords_file, "r")) == NULL) {
		fprintf(stderr, "Failed to open %s", stopwords_file);
		perror(" ");
		exit(EXIT_FAILURE);
	}
	const char *del = " ,\n\t";
	char *		word;
	char *		token = NULL, *line = NULL;
	size_t		len = 0;
	ssize_t		chars = 0;
	while ((chars = getline(&line, &len, fp)) != -1) {
		token = strtok(line, del);
		if (token == NULL) {
			perror("Failed to tokenize string: ");
			return -1;
		}

		word = malloc(strlen(token) + 1);
		strcpy(word, token);
		map_insert(stopwords, word, word);

		while ((token = strtok(NULL, del)) != NULL) {
			word = malloc(strlen(token) + 1);
			strcpy(word, token);
			map_insert(stopwords, word, word);
		}
	}
	free(line);
	fclose(fp);
	return 0;
}

Vectorizer *vectorizer_init(int files_count, int type)
{
	Vectorizer *v = malloc(sizeof(Vectorizer));
	v->documents_count = files_count;
	v->word_frequencies = map_init(files_count, hash_str, compare_str, free, vector_delete);
	v->type = type;
	pthread_mutex_init(&(v->mutex), NULL);
	if (type != 0)
		v->words_idf = map_init(5 * files_count, hash_str, compare_str, free, idf_elem_delete);
	else
		v->words_idf = NULL;
	return v;
}

void vectorizer_delete(Vectorizer *v)
{
	map_delete(v->word_frequencies);
	map_delete(v->features);

	if (v->words_idf != NULL)
		map_delete(v->words_idf);
	free(v);
}
int cnt;

void vectorizer_fit_transform(Vectorizer *vectorizer, char *path, int max_features)
{
#ifdef SHOW_PROGRESS
	printf("Finding the frequency of each word in each document...\n");
	cnt = 0;
#endif
	vectorizer_construct(vectorizer, path);
	printf("%c[2K\rDone\n", 27);
	if (vectorizer->type == TFIDF) {
		printf("Calculating term frequency values...\n");
		compute_tf_values(vectorizer);
		printf("Calculating inverse-document frequency values...\n");
		compute_idf_values(vectorizer);
		printf("Calculating tfidf values...\n");
		compute_tfidf_values(vectorizer);
		printf("Reducing the total features down to %d...\n", max_features);
		vectorizer_reduce_features(vectorizer, max_features);
	}
	// int default_feature_reductions = (int)((double) 2/3 *
	// vectorizer->words_idf->total_items);
	// vectorizer_transform(vectorizer, default_feature_reductions);
}

double *vectorizer_get_vector(Vectorizer *vectorizer, char *document1, char *document2)
{
	double *x_vector = calloc(2 * vectorizer->max_features, sizeof(double));

	pthread_mutex_lock(&vectorizer->mutex);
	struct vector *			vec;
	struct vectorizer_elem *elem;
	struct triple *			t;
	/* Get the first document in the sparse matrix */
	if ((vec = map_find(vectorizer->word_frequencies, document1)) != NULL) {
		/* Get its tfidf values for every word */
		for (int n = 0; n < vector_size(vec); ++n) {
			elem = (struct vectorizer_elem *) vector_get(vec, n);
			/* If it belogns to the reduced features, store the tfidf value */
			if ((t = map_find(vectorizer->features, elem->word)) != NULL) {
				t->b_d = elem->tfidf;
			}
		}
	}
	/* Get the second document in the sparse matrix */
	if ((vec = map_find(vectorizer->word_frequencies, document2)) != NULL) {
		for (int n = 0; n < vector_size(vec); ++n) {
			/* Get its tfidf values for every word */
			elem = (struct vectorizer_elem *) vector_get(vec, n);
			/* If it belogns to the reduced features, store the tfidf value */
			if ((t = map_find(vectorizer->features, elem->word)) != NULL) {
				t->c_d = elem->tfidf;
			}
		}
	}

	/* Now that the tfidf values of the reduced features are updated, create the word vector */
	for (struct triple *iter = map_begin(vectorizer->features); iter != NULL;
		 iter = map_advance(vectorizer->features)) {
		x_vector[iter->a_i] = (double) iter->b_d;
		x_vector[vectorizer->max_features + iter->a_i] = (double) iter->c_d;
		iter->b_d = 0.0;
		iter->c_d = 0.0;
	}
	pthread_mutex_unlock(&vectorizer->mutex);
	return x_vector;
}

void vectorizer_construct(Vectorizer *vectorizer, char *path)
{
	struct dirent *pDirent;
	char *		   subdir, *path_to_file;

	DIR *pDir = opendir(path);
	if (pDir == NULL) {
		perror("vectorizer_construct: can't open directory");
		exit(EXIT_FAILURE);
	}
	while ((pDirent = readdir(pDir)) != NULL) {
		if (strcmp(pDirent->d_name, ".") != 0 && strcmp(pDirent->d_name, "..") != 0) {
			if (pDirent->d_type == DT_DIR) {
				subdir = calloc(strlen(path) + 2 + strlen(pDirent->d_name), sizeof(char));
				/* Construct the path to the subdirectory */
				strcat(subdir, path);
				if (path[strlen(path) - 1] != '/')
					strcat(subdir, "/");
				strcat(subdir, pDirent->d_name);
				/* Recurse into the subdirectory */
				vectorizer_construct(vectorizer, subdir);
				free(subdir);
			}
			else if (pDirent->d_type == DT_REG && (strcmp(strrchr(pDirent->d_name, '.'), ".json") == 0)) {
				char *temp_path = calloc(strlen(path) + 1, sizeof(char));
				strcpy(temp_path, path);

				char *website = strtok(temp_path, "/");
				char *prev_website;
				do {
					prev_website = website;
					website = strtok(NULL, "/");
				} while (website != NULL);
				website = prev_website;
				/* path for file */
				path_to_file = malloc(strlen(path) + 2 + strlen(pDirent->d_name));
				strcpy(path_to_file, path);

				if (path[strlen(path_to_file) - 1] != '/')
					strcat(path_to_file, "/");

				strcat(path_to_file, pDirent->d_name);

				parse_json(vectorizer, path_to_file, pDirent->d_name, website);
#ifdef SHOW_PROGRESS
				printf("\r%.1f%%", rescale_lo_hi(cnt++, 0, vectorizer->documents_count, 0, 100));
				fflush(stdout);
#endif
				free(path_to_file);
				free(temp_path);
			}
		}
	}
	closedir(pDir);
}

void word_frequencies_add_value(struct hash_map *word_frequencies, char *file, char *word)
{
	struct vector *vec;
	/* Search the file name in the hash table of frequencies */
	if ((vec = map_find(word_frequencies, file)) != NULL) {
		int found = 0;
		for (int i = 0; i < vector_size(vec) && found == 0; ++i) {
			struct vectorizer_elem *elem = (struct vectorizer_elem *) vector_get(vec, i);
			if (strcmp(elem->word, word) == 0) {
				elem->frequency++;
				found = 1;
			}
		}
		if (found == 0) {
			struct vectorizer_elem *b = vectorizer_elem_init(file, word, 1);
			vector_push_back(vec, b);
		}
	}
	else {
		vec = vector_init(0, delete_vectorizer_elem);
		struct vectorizer_elem *b = vectorizer_elem_init(file, word, 1);
		vector_push_back(vec, b);
		char *key = malloc(strlen(file) + 1);
		strcpy(key, file);
		map_insert(word_frequencies, key, vec);
	}
}

void vectorizer_reduce_features(Vectorizer *vectorizer, int max_features)
{
	vectorizer->max_features = max_features;
	if (max_features > vectorizer->words_idf->total_items) {
		vectorizer->max_features = vectorizer->words_idf->total_items;
	}
	if (vectorizer->type == TFIDF) {
		tfidf_reduce_features(vectorizer);
	}
}

void parse_json(Vectorizer *vectorizer, char *path, char *id, char *site)
{
	char *	str, *line = NULL, *spec_title = NULL, *spec_value = NULL;
	ssize_t read;
	size_t	len = 0;
	FILE *	fp = fopen(path, "r");
	if (fp == NULL) {
		perror("parse_json: can't open directory");
		exit(EXIT_FAILURE);
	}

	strip_ext(id);	  // Remove '.json' from the filename
	char *file = malloc(strlen(site) + strlen(id) + 3);
	strcat(strcat(strcpy(file, site), "//"), id);

	/* Begin reading the json file */
	while ((read = getline(&line, &len, fp)) != -1) {
		// take the line of the file
		str = line;
		if (line[0] != '{' && line[0] != '}')	 // ignore { and }
		{
			skip_whitespace(str);
			// format sequence
			if (str[0] == '"') {
				/* Get the title of the spec in the current line */
				spec_title = extract_spec_title(str);
				free(spec_title);
				/* Get the spec's value in the current line, or in
				 * next lines if there are multiple values */
				spec_value = extract_spec_value(str, fp);

				char *del = " \n";
				spec_value = strrem_special_characters(spec_value);
				if (strlen(spec_value) == 1) {
					free(spec_value);
					continue;
				}

				/* Tokenize the value */
				char *token;
				for (token = strtok(spec_value, del); token; token = strtok(NULL, del)) {
					if (strlen(token) > 3) {
						if (map_find(stopwords, token) == NULL) { /* Proceed only if the token isn't
																	 a stopword */
							word_frequencies_add_value(vectorizer->word_frequencies, file, token);
							if (vectorizer->type == TFIDF) {
								words_idf_add_value(vectorizer->words_idf, file, token);
							}
						}
					}
				}
				free(spec_value);
			}
		}
	}
	free(file);
	free(line);
	fclose(fp);
}