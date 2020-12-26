#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <dirent.h>

#include <sys/types.h>
#include "../include/map.h"
#include "../include/clique.h"
#include "../include/util.h"
#include "../include/list.h"
#include "../include/dataset_parsing.h"

struct hash_map *stopwords;

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
	int size;
	if ((size = pick_the_buckets(argc, argv)) <= 0)
		return -1;

	/* Safe assignments, error checking was done in pick_the_buckets */
	char *data_path = argv[1];	
	char *relations_file = argv[2];
	printf("Hash table size: %d \n",size );

	/* Initialize the hash table */
	struct hash_map *map = map_init(size, hash_str, NULL, free, delete_clique); 
	/* Parse the json files */
	read_data_files(map,size,data_path);

	/* Parse the relations and change the cliques accordingly */
	read_relations(map,relations_file);
	
	/* Print the results of the cliques */
	int l = print_results(map);
	printf("%d \n",l);

	/* Free the allocated memory of the hash table */
	map_delete(map);
	
	return 0;
}
