#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <dirent.h>
#include <time.h>
#include <sys/types.h>
#include "../include/map.h"
#include "../include/clique.h"
#include "../include/util.h"
#include "../include/list.h"
#include "../include/dataset_parsing.h"
#include "../include/vectorizer.h"
#include "../include/classifier.h"

struct hash_map *stopwords;
double learning_rate;



int main(int argc, char *argv[]) {
	srand(time(NULL));
	int size;
	if ((size = pick_the_buckets(argc, argv)) <= 0)
		return -1;

	/* Safe assignments, error checking was done in pick_the_buckets */
	char *data_path = argv[1];	
	char *relations_file = argv[2];
	learning_rate = 0.1;
	int max_features = 5000;
	// train_test_split(model,relations_file,0.6,0.4);

	/* Construct the stopwords dictionary */
	stopwords = map_init(37, hash_str, compare_str, free, NULL);
	get_stopwords("Datasets/stopwords.txt");

	/* Initialize the hash table */
	struct hash_map *map = map_init(size, hash_str, compare_str, free, delete_clique);

	/* Parse the json files */
	printf("Constructing clique relationships...\n");
	// read_data_files(map, size, data_path);

	// read_relations(map,relations_file);

	// make_the_files(map);

	printf("Generating the word vectors...\n");
	struct vectorizer *tfidf = vectorizer_init(size, 1);	// 1 means tfidf instead of bow
	vectorizer_fit_transform(tfidf, data_path, max_features);

	printf("Reading the binary labels...\n");
	
	int n_positive_labels, n_negative_labels;
	if ((n_positive_labels = get_label_count("Datasets/positive_relations.csv")) < 0)
		exit(EXIT_FAILURE);
	
	char **positive_labels = malloc(n_positive_labels * sizeof(char*));
	
	if ((n_negative_labels = get_label_count("Datasets/negative_relations.csv")) < 0)
		exit(EXIT_FAILURE);
	
	char **negative_labels = malloc(n_negative_labels * sizeof(char*));

	if ((load_labels("Datasets/positive_relations.csv", positive_labels, "Datasets/negative_relations.csv", negative_labels)) < 0)
		exit(EXIT_FAILURE);
	
	struct labels_sets *sets;
	if ((sets = train_test_split(positive_labels, n_positive_labels, negative_labels, n_negative_labels, 0.6, 0.2, 1)) == NULL)
		exit(EXIT_FAILURE);
	
	printf("Initializing the logistic regression model\n");
	struct LogisticRegressor *model = Logistic_Regression_Init();
	Logistic_Regression_fit(model, tfidf);
	

	printf("Training the model...\n");
	train(model, sets->train_set, sets->n_train_labels);


	vectorizer_delete(tfidf);
	/*
	/* Parse the relations and change the cliques accordingly */
	/* Print the results of the cliques */

	
	/* Free the allocated memory of the hash table */
	map_delete(map);
	map_delete(stopwords);
	
	return 0;
}






/*
struct all_info *start=NULL;

start=malloc(sizeof(struct all_info));
start->size=0;
start->first=NULL;

read_relations_2(map,relations_file,start);
int l = print_results(map);
printf("THe integer is %d \n",l);

struct list_negative *tranverse=start->first;

int ok=print_negative_results(map);

printf("%d\n", ok);
int sum=0;

while (tranverse!=NULL)
{
	printf("%d %d \n",tranverse->ptr_1->size,tranverse->ptr_2->size );
	sum=sum + (tranverse->ptr_1->size * tranverse->ptr_2->size);
	tranverse=tranverse->next;
}

printf("Sum is %d \n",sum );
*/