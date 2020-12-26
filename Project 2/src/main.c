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

struct hash_map *stopwords;
double learning_rate;

int get_stopwords(char *stopwords_file) {
	FILE *fp;
	if ((fp = fopen(stopwords_file, "r")) == NULL) {
		fprintf(stderr,"Failed to open %s", stopwords_file);
		return -1;
	}
    const char *del = " ,\n\t";
	char *word;
    char *token = NULL, *line = NULL;
	size_t len = 0, chars = 0;
	while((chars = getline(&line, &len, fp)) != EOF) {
		token = strtok(line, del);
		if (token == NULL) {
			fprintf(stderr, "Failed to tokenize string");
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

int main(int argc, char *argv[]) {
	srand(time(NULL));

	// row = 0;
	// column = 0;
	int size;
	if ((size = pick_the_buckets(argc, argv)) <= 0)
		return -1;

	/* Safe assignments, error checking was done in pick_the_buckets */
	char *data_path = argv[1];	
	char *relations_file = argv[2];
	// learning_rate = 0.01;
	// int max_features = atoi(argv[3]);
	// train_test_split(model,relations_file,0.6,0.4);
	
	/* Initialize the hash table */
	struct hash_map *map = map_init(size, hash_str, compare_str, free, delete_clique);

	// /* Construct the stopwords dictionary */
	// stopwords = map_init(37, hash_str, compare_str, free, NULL);
	// get_stopwords("Datasets/stopwords.txt");

	// printf("New hash table size: %d \n",map->size);
	// /* Parse the json files */
	// printf("Constructing clique relationships...\n");
	read_data_files(map, size, data_path);

	// printf("Generating the word vectors...\n");

	//struct vectorizer *tfidf = vectorizer_init(size, 1);	// 1 means tfidf instead of bow
	//vectorizer_fit_transform(tfidf, data_path, max_features);

	// printf("Initializing the logistic regression model\n");
	//struct LogisticRegressor *model = Logistic_Regression_Init();

	// printf("Split train and test datasets\n");
	//train_test_split(model, relations_file, 0.6, 0.4);

	// printf("Fitting the data into our model\n");
	//Logistic_Regression_fit(model, tfidf);

	// printf("Training...\n");
	// map_print(tfidf->features);
	//train_test_split(model, relations_file, 0.6, 0.4);
	//train(model, "Datasets/train_labels.csv");

	//vectorizer_delete(tfidf);

	// /* Parse the relations and change the cliques accordingly */
	read_relations(map,relations_file);
	/* Print the results of the cliques */

	struct all_info *start=NULL;

	start=malloc(sizeof(struct all_info));
	start->size=0;
	start->first=NULL;

	read_relations_2(map,relations_file,start);
	int l = print_results(map);
	printf("THe integer is %d \n",l);

	struct list_negative *tranverse=start->first;

	//int ok=print_negative_results(map);

	//printf("%d\n", ok);

	make_the_files(map);

	int sum=0;

	while (tranverse!=NULL)
	{
		// printf("%d %d \n",tranverse->ptr_1->size,tranverse->ptr_2->size );
		sum=sum + (tranverse->ptr_1->size * tranverse->ptr_2->size);
		tranverse=tranverse->next;
	}

	printf("Sum is %d \n",sum );
	/* Free the allocated memory of the hash table */
	// map_delete(map);
	// map_delete(stopwords);
	
	return 0;
}