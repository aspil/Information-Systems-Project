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


extern struct hash_map *stopwords;

int get_stopwords(char *stopwords_file) {
	FILE *fp;
	if ((fp = fopen(stopwords_file, "r")) == NULL) {
		fprintf(stderr,"Failed to open %s", stopwords_file);
		return -1;
	}
    const char *del = " ,\n\t";
	char *word;
    char *token = NULL, *line = NULL;
	size_t len = 0;
	ssize_t chars = 0;
	while((chars = getline(&line, &len, fp)) != -1) {
		token = strtok(line, del);
		if (token == NULL) {
			perror("Failed to tokenize string: ");
			return -1;
		}

		word = malloc(strlen(token)+ 1);
		strcpy(word,token);
		map_insert(stopwords, word, word);

		while ((token = strtok(NULL,del)) != NULL) {
			word = malloc(strlen(token)+ 1);
			strcpy(word,token);
			map_insert(stopwords, word, word);
		}
    }
	free(line);
	fclose(fp);
	return 0;
}
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

/* ---------------------- struct feature functions ---------------------- */
struct feature* feature_init(char *str, double p) {
	struct feature *f = malloc(sizeof(struct feature));
	f->data = str;
	// f->data = malloc(sizeof(str));
	// strcpy(f->data, str);
	f->priority = p;
	return f;
}

int compare_features(void *f1, void *f2) {
	if ((double)((struct feature*)f1)->priority > (double)((struct feature*)f2)->priority)
		return 1;
	else
		return -1;
}

void feature_delete(void *f) {
	struct feature *feat = (struct feature*) f;
	// free(feat->data);
	free(feat);
}

/* ---------------------- struct vectorizer functions ---------------------- */

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
	
	map_delete(v->features);

	if (v->words_idf != NULL)
		map_delete(v->words_idf);
	
	free(v);
}

void vectorizer_fit_transform(struct vectorizer *vectorizer, char *path, int max_features) {
	vectorizer_fit(vectorizer, path);
	vectorizer_transform(vectorizer, max_features);

	// int default_feature_reductions = (int)((double) 2/3 * vectorizer->words_idf->total_items);
	// vectorizer_transform(vectorizer, default_feature_reductions);
}

void vectorizer_transform(struct vectorizer *vectorizer, int max_features) {
	if (vectorizer->words_idf != NULL) {
		compute_tf_values(vectorizer);
		compute_idf_values(vectorizer);
		compute_tfidf_values(vectorizer);
		reduce_features(vectorizer, max_features);
	}
}

void vectorizer_fit(struct vectorizer *vectorizer, char *path) {
	vectorizer_construct(vectorizer, path);
}

double* vectorizer_get_vector(struct vectorizer *vectorizer, char *document1, char *document2) {
	int document_words_count;
	double *x_vector = calloc(2 * vectorizer->max_features, sizeof(double));
	
	struct vector *vec;
	struct vectorizer_elem *elem;
	struct triple *t;
	if ((vec = map_find(vectorizer->word_frequencies, document1)) != NULL) {
		document_words_count = vector_size(vec);
		for (int n = 0; n < document_words_count; ++n) {
			elem = (struct vectorizer_elem*) vector_get(vec,n);
			if ((t = map_find(vectorizer->features, elem->word)) != NULL) {
				t->b_d = elem->tfidf;
			}
		}
	}

	if ((vec = map_find(vectorizer->word_frequencies, document2)) != NULL) {
		document_words_count = vector_size(vec);
		for (int n = 0; n < document_words_count; ++n) {
			elem = (struct vectorizer_elem*) vector_get(vec,n);
			if ((t = map_find(vectorizer->features, elem->word)) != NULL) {
				t->c_d = elem->tfidf;
			}
		}
	}
	/* TODO: change to map_begin and map_advance */
	for (struct triple *iter = map_begin(vectorizer->features); iter != NULL; iter = map_advance(vectorizer->features)) {
		x_vector[iter->a_i] = (double) iter->b_d;
		x_vector[vectorizer->max_features + iter->a_i] = (double) iter->c_d;
		iter->b_d = 0.0;
		iter->c_d = 0.0;
	}

	return x_vector;
}

void vectorizer_construct(struct vectorizer *vectorizer, char *path) {
	struct dirent *pDirent;
	char *subdir, *path_to_file;

	DIR *pDir = opendir(path); //anoigma tou path
	if (pDir == NULL) {
		perror("vectorizer_construct: can't open directory");
		exit(EXIT_FAILURE);
	}
	while ((pDirent = readdir(pDir)) != NULL) 
	{
		if (strcmp(pDirent->d_name, ".") != 0 && strcmp(pDirent->d_name, "..") != 0) //diabase ola ektos apo tis . kai ..
		{
			if (pDirent->d_type  ==  DT_DIR) {
				subdir = calloc(strlen(path) + 2 + strlen(pDirent->d_name), sizeof(char));
				/* Construct the path to the subdirectory */
				strcat(subdir,path);
				if (path[strlen(path)-1] != '/')
					strcat(subdir,"/");
				strcat(subdir,pDirent->d_name);
				/* Recurse into the subdirectory */
				vectorizer_construct(vectorizer, subdir);
				free(subdir);
			}
			else if (pDirent->d_type == DT_REG && (strcmp(strrchr(pDirent->d_name,'.'), ".json")  ==  0)) {

				char *temp_path = calloc(strlen(path)+1,sizeof(char));
				strcpy(temp_path, path);
				
				char *website = strtok(temp_path,"/");
				char *prev_website;
				do {
					prev_website = website;
					website = strtok(NULL,"/");
				} while (website != NULL);
				website = prev_website;
				/* path for file */
				path_to_file = malloc(strlen(path) + 2 + strlen(pDirent->d_name));
				strcpy(path_to_file, path);
				
				if (path[strlen(path_to_file)-1] != '/')
					strcat(path_to_file,"/");

				strcat(path_to_file, pDirent->d_name);

				parse_json(vectorizer, path_to_file, pDirent->d_name, website);

				free(path_to_file);
				free(temp_path);
			}
		}
	}
	closedir(pDir);
}

void word_frequencies_add_value(struct hash_map *word_frequencies, char *file, char *word) {
	struct vector *vec;
	/* Search the file name in the hash table of frequencies */
	if ((vec = map_find(word_frequencies, file)) != NULL) {
		int found = 0;
		for (int i = 0; i < vector_size(vec) && found == 0; ++i) {
			struct vectorizer_elem* elem = (struct vectorizer_elem*) vector_get(vec,i);
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
		char *key = malloc(strlen(file)+1);
		strcpy(key, file);
		map_insert(word_frequencies, key, vec);
	}
}

void words_idf_add_value(struct hash_map *words_idf, char *file, char *word) {
	struct idf_elem *elem = NULL;
	
	if ((elem = map_find(words_idf, word)) != NULL) {
		/* Only append the file if it hasn't been appended yet */
		if (strcmp(list_last_value(elem->files), file) != 0) {
			char *filename = malloc(strlen(file)+1);
			strcpy(filename, file);
			list_append(elem->files, filename);
		}
	}
	else {
		
		char *key = malloc(strlen(word)+1);
		strcpy(key, word);

		char *filename = malloc(strlen(file)+1);
		strcpy(filename, file);

		elem = idf_elem_init(key); // free will be passed to elem's list which holds string elements
		list_append(elem->files, filename);
		
		map_insert(words_idf, key, elem);
	}
}

void compute_tf_values(struct vectorizer *vectorizer) {
	// if (vectorizer->words_idf == NULL) {
	// 	struct hash_map *avg_tf_map = map_init(hash_str, compare_str, NULL, NULL);
		
	// 	for (void *iter = map_begin(vectorizer->word_frequencies); iter != NULL; iter = map_advance(vectorizer->word_frequencies)) {
	// 		struct vector *vec = (struct vector*) iter;
	// 		document_words_count = vector_size(vec);
			
	// 		for (int i = 0; i < document_words_count; ++i) {
	// 			struct vectorizer_elem *elem = (struct vectorizer_elem*) vector_get(vec,i);
	// 			if (map_find(avg_tf_map, elem->word))
	// 			// /* Calculate the tf value for the current word in the current file.
	// 			// * IMPORTANT: frequency member variable is now inactive;
	// 			// * use only tfidf to avoid undefined behavior. */
	// 			// elem->tfidf = (double) elem->frequency / document_words_count;	/* elem->tfidf currently holds tf value only */
	// 			// printf("tf: %sf\n",elem->tfidf);
	// 		}
	// 	}
	// 	return avg_tf_map;
	// }
	// else {
		int document_words_count = 0;
		for (void *iter = map_begin(vectorizer->word_frequencies); iter != NULL; iter = map_advance(vectorizer->word_frequencies)) {
			struct vector *vec = (struct vector*) iter;
			document_words_count = vector_size(vec);
			
			for (int i = 0; i < document_words_count; ++i) {
				struct vectorizer_elem *elem = (struct vectorizer_elem*) vector_get(vec,i);
				/* Calculate the tf value for the current word in the current file.
				* IMPORTANT: frequency member variable is now inactive;
				* use only tfidf to avoid undefined behavior. */
				elem->tfidf = (double) elem->frequency / document_words_count;
				// printf("tf: %sf\n",elem->tfidf);
			}
		}
	// }
}

void compute_idf_values(struct vectorizer *vectorizer) {
	for (void *iter = map_begin(vectorizer->words_idf); iter != NULL; iter = map_advance(vectorizer->words_idf)) {
		struct idf_elem *elem = (struct idf_elem*)iter;
		elem->idf = log2((double) vectorizer->documents_count / list_size(elem->files));
	}
}
	
void compute_tfidf_values(struct vectorizer *vectorizer) {
	int document_words_count;
	struct idf_elem *idf_val;
	double idf;
	for (void *iter = map_begin(vectorizer->word_frequencies); iter != NULL; iter = map_advance(vectorizer->word_frequencies)) {
		struct vector *vec = (struct vector*) iter;
		document_words_count = vector_size(vec);
		
		for (int i = 0; i < document_words_count; ++i) {
			struct vectorizer_elem *elem = (struct vectorizer_elem*) vector_get(vec,i);

			if ((idf_val = (struct idf_elem*) map_find(vectorizer->words_idf, elem->word)) != NULL) {
				idf = idf_val->idf;
				elem->tfidf *= idf;	/* Now elem has the tfidf value */
				idf_val->average_tfidf += elem->tfidf; /* Add up to the average tfidf of this word */
			}
		}
	}
}

void reduce_features(struct vectorizer *vectorizer, int max_features) {
	for (void *iter = map_begin(vectorizer->words_idf); iter != NULL; iter = map_advance(vectorizer->words_idf)) {
		struct idf_elem *elem = (struct idf_elem*)iter;
		elem->average_tfidf /= vectorizer->documents_count;
	}
	vectorizer->max_features = max_features;
	if (max_features > vectorizer->words_idf->total_items) {
		vectorizer->max_features = vectorizer->words_idf->total_items;
	}
	struct heapq *heap = heapq_init(compare_features, feature_delete); 	/* free will be used only on struct feature */

	for (void *iter = map_begin(vectorizer->words_idf); iter != NULL; iter = map_advance(vectorizer->words_idf)) {
		struct feature *feat = feature_init(((struct idf_elem*) iter)->word, ((struct idf_elem*) iter)->average_tfidf);
		heapq_insert(heap, feat);
	}

	vectorizer->features = map_init(max_features, hash_str, compare_str, free, free);

	int column = 0;
	for (int i = 0; i < vectorizer->max_features; ++i) {
		struct feature *extracted = heapq_peek(heap);
		
		char *key = malloc(strlen(extracted->data)+1);
		strcpy(key, extracted->data);

		struct triple *t = malloc(sizeof(struct triple));
		t->a_i = column;
		t->b_d = 0.0;
		t->c_d = 0.0;
		map_insert(vectorizer->features, key, t);
		

		heapq_extract(heap);
		free(extracted);
		column++;
	}

	heapq_delete(heap);
}

void parse_json(struct vectorizer *vectorizer, char *path, char *id, char *site) {
	char *str,*line = NULL, *spec_title = NULL, *spec_value = NULL;
	ssize_t read;
	size_t len = 0;
	FILE *fp = fopen(path,"r");
	if (fp  ==  NULL) {
		perror("parse_json: can't open directory");
		exit(EXIT_FAILURE);
	}

	strip_ext(id);	// Remove '.json' from the filename
	char *file = malloc(strlen(site) + strlen(id) + 3);
	strcat(strcat(strcpy(file,site), "//"), id);

	/* Begin reading the json file */
	while ((read = getline(&line, &len, fp)) != -1) 
	{
		// take the line of the file
		str = line;
		if (line[0] != '{' && line[0] != '}') // ignore { and }
		{
			skip_whitespace(str);
			// format sequence 
			if (str[0] == '"')
			{
				/* Get the title of the spec in the current line */
				spec_title = extract_spec_title(str);
				free(spec_title);
				/* Get the spec's value in the current line, or in next lines if there are multiple values */
				spec_value = extract_spec_value(str, fp);
				
				char *del = " \n";
				spec_value = preprocess_text(spec_value);
				if (strlen(spec_value) == 1) {
					free(spec_value);
					continue;
				}
				
				/* Tokenize the value */
				char *token;
				for (token = strtok(spec_value,del); token; token = strtok(NULL, del)) {
					if (strlen(token) > 3) {
						if (map_find(stopwords, token) == NULL) {	/* Proceed only if the token isn't a stopword */
							word_frequencies_add_value(vectorizer->word_frequencies, file, token);
							if (vectorizer->words_idf != NULL) {
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