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
double learning_rate = 0.0001;

void free_datasets(struct labels_sets *s);

int main(int argc, char *argv[]) {
	srand(time(NULL));
	int size;
	int max_features,print;
	if ((size = pick_the_buckets_and_features(argc, argv, &max_features,&print))== -1)
		return -1;

	else if (size>0)
	{

	/* Safe assignments, error checking was done in pick_the_buckets */
	char *data_path = argv[1];	
	char *relations_file = argv[2];
	learning_rate = 0.0001;

	/* Construct the stopwords dictionary */
	stopwords = map_init(37, hash_str, compare_str, free, NULL);
	get_stopwords("Datasets/stopwords.txt");

	/* Initialize the hash table */
	struct hash_map *map = map_init(size, hash_str, compare_str, free, delete_clique);

	/* Parse the json files */
	printf("Constructing clique relationships...\n");
	
	read_data_files(map, size, data_path);

	read_relations(map,relations_file);

	make_the_files(map);
	map_delete(map);
	printf("Generating the word vectors...\n");

	int size_of_vector=count_json_files(data_path);
	struct vectorizer *tfidf = vectorizer_init(size_of_vector, 1);	// 1 means tfidf instead of bow
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
	
	printf("Test labels are : %d\n",sets->n_test_labels );

	//write the weights in a separate file 

	FILE *fp = fopen("weights.txt","w");
	fprintf(fp,"%d\n",model->n_weights);
	for (int i = 0; i < model->n_weights; ++i) 
	{
		fprintf(fp,"%f\n",model->weights[i]);
		//printf("%f\n",model->weights[i]);
	 }
	 fclose(fp);

	 //write the testing labels to a separate file
	FILE *fp_1 = fopen("test.csv","w");
	fprintf(fp_1,"%d\n",sets->n_test_labels);
	for (int i = 0; i < sets->n_test_labels; ++i) 
	{
		fprintf(fp_1,"%s%d\n",sets->test_set_input[i],sets->test_set_labels[i]);
		
	}
	fclose(fp_1);

	print_cliques(print);


	Logistic_Regression_Delete(model);

	/* Free the allocated memory */
	//free(predictions);
	free_datasets(sets);
	vectorizer_delete(tfidf);
	
	map_delete(stopwords);
	}
	else if (size==0)
	{
		char *data_path = "Datasets";

		int size_of_vector=count_json_files(data_path);

		printf("Generating the word vectors...\n");
		// 1 means tfidf instead of bow
		//vectorizer_fit_transform(tfidf, data_path, max_features);
		//testing area
		//printf("Welcome to the testing area\n");

		stopwords = map_init(37, hash_str, compare_str, free, NULL);
		get_stopwords("Datasets/stopwords.txt");

		printf("Testing the model...\n");
		printf("Initializing the logistic regression model\n");
	
		struct LogisticRegressor *model = Logistic_Regression_Init();

		FILE *fp = fopen("weights.txt","r");

		int i=0;
		char *line;
    	size_t len = 0;
   	 	ssize_t read;

		read = getline(&line, &len, fp);

		if (read<0)
		{
			printf("Wrong file\n");
			return -1;
		}

		model->n_weights=atoi(line);
		model->weights=malloc(sizeof(double)*model->n_weights);

		struct vectorizer *tfidf = vectorizer_init(size_of_vector, 1);

		vectorizer_fit_transform(tfidf, data_path, (model->n_weights-1)/2);

		model->vect=tfidf;

		int *test_labels,n_test_labels;

		while ((read = getline(&line, &len, fp)) != -1) 
		{	
			model->weights[i]=atof(line); //pass the weights from training
			i++;
		}

		fclose(fp);

		fp = fopen("test.csv","r");

		read = getline(&line, &len, fp);

		if (read<0)
		{
			printf("Wrong file\n");
			return -1;
		}

		n_test_labels=atoi(line);

		test_labels=malloc(sizeof(int)*n_test_labels);

		char **test_set=malloc(sizeof(char*)*n_test_labels);


		i=0;

		while ((read = getline(&line, &len, fp)) != -1) 
		{	
			get_line_without_end_line(line); //take the relations

			test_set[i]=malloc(strlen(line)+1);

			strcpy(test_set[i],line);

			test_labels[i]=line[strlen(line)-1]- '0';

			i++;

		}

		int *predictions = test(model,test_set,n_test_labels);
		
		printf("Accuracy: %f\n", accuracy_score(test_labels, predictions,n_test_labels));
		printf("Precision: %f\n", precision_score(test_labels, predictions,n_test_labels));
		printf("Recall: %f\n", recall_score(test_labels, predictions,n_test_labels));
		printf("F1: %f\n", f1_score(test_labels, predictions,n_test_labels));


		Logistic_Regression_Delete(model);
		
		free(line);

		for (int i = 0; i < n_test_labels; ++i)
		{
			free(test_set[i]);
		}
		
	


		free(test_set);

		free(test_labels);



	}
	else
	{	//validate area
		printf("Welcome to the validate area\n");

		char *data_path = "Datasets";

		int size=count_json_files(data_path);

		printf("Generating the word vectors...\n");
		// 1 means tfidf instead of bow
		//vectorizer_fit_transform(tfidf, data_path, max_features);
		//testing area
		//printf("Welcome to the testing area\n");

		stopwords = map_init(37, hash_str, compare_str, free, NULL);
		get_stopwords("Datasets/stopwords.txt");

		printf("Testing the model...\n");
		printf("Initializing the logistic regression model\n");
	
		struct LogisticRegressor *model = Logistic_Regression_Init();

		FILE *fp = fopen("weights.txt","r");

		int i=0;
		char *line;
    	size_t len = 0;
   	 	ssize_t read;

		read = getline(&line, &len, fp);

		if (read<0)
		{
			printf("Wrong file\n");
			return -1;
		}

		model->n_weights=atoi(line);
		model->weights=malloc(sizeof(double)*model->n_weights);

		struct vectorizer *tfidf = vectorizer_init(size, 1);

		vectorizer_fit_transform(tfidf, data_path, (model->n_weights-1)/2);

		model->vect=tfidf;

		int *test_labels,n_test_labels;

		while ((read = getline(&line, &len, fp)) != -1) 
		{	
			model->weights[i]=atof(line); //pass the weights from training
			i++;
		}

		fclose(fp);

		int j=0;

		i=0;

		fp = fopen("Datasets/positive_relations.csv","r");

		while ((read = getline(&line, &len, fp)) != -1) 
		{
			i++;
		}

		fclose(fp);

		fp = fopen("Datasets/negative_relations.csv","r");		

		while ((read = getline(&line, &len, fp)) != -1) 
		{
			j++;
		}
		fclose(fp);
		
		fp = fopen("Datasets/positive_relations.csv","r");

		n_test_labels=j+i;

		test_labels=malloc(sizeof(int)*n_test_labels);

		char **test_set=malloc(sizeof(char*)*n_test_labels);

		int counter=0;

		while ((read = getline(&line, &len, fp)) != -1) 
		{	
			get_line_without_end_line(line); //take the relations

			test_set[counter]=malloc(strlen(line)+1);

			strcpy(test_set[counter],line);

			test_labels[counter]=line[strlen(line)-1]- '0';

			counter++;

		}
		fclose(fp);

		fp = fopen("Datasets/negative_relations.csv","r");

		while ((read = getline(&line, &len, fp)) != -1) 
		{	
			get_line_without_end_line(line); //take the relations

			test_set[counter]=malloc(strlen(line)+1);

			strcpy(test_set[counter],line);

			test_labels[counter]=line[strlen(line)-1]- '0';

			counter++;

		}
		fclose(fp);


		int *predictions = test(model,test_set,n_test_labels);
		
		printf("Accuracy: %f\n", accuracy_score(test_labels, predictions,n_test_labels));
		printf("Precision: %f\n", precision_score(test_labels, predictions,n_test_labels));
		printf("Recall: %f\n", recall_score(test_labels, predictions,n_test_labels));
		printf("F1: %f\n", f1_score(test_labels, predictions,n_test_labels));

		Logistic_Regression_Delete(model);
		
		free(line);

		for (int i = 0; i < n_test_labels; ++i)
		{
			free(test_set[i]);
		}

		free(test_set);

		free(test_labels); 
	}
	return 0;
}

void free_datasets(struct labels_sets *s) {
	for (int i = 0; i < s->n_train_labels; ++i) {
		free(s->train_set[i]);
	};
	free(s->train_set);

	for (int i = 0; i < s->n_test_labels; ++i) {
		free(s->test_set_input[i]);
	};
	free(s->test_set_input);
	free(s->test_set_labels);

	for (int i = 0; i < s->n_validate_labels; ++i) {
		free(s->validate_set[i]);
	};
	free(s->validate_set);

	free(s);
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