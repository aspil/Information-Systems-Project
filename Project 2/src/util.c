#include <dirent.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "../include/util.h"
#include "../include/map.h"
#include "../include/vector.h"
#include "../include/clique.h"
#include "../include/dataset_parsing.h"

void strip_ext(char *fname) {
	char *end = fname + strlen(fname);
	while (end > fname && *end != '.')
		--end;
	
	if (end > fname)
		*end = '\0';
	return;
}

void get_line_without_end_line(char *str)
{
	int i = 0;
	while (str[i] != '\n')
		i++;

	str[i] = '\0';
	return;
}

int count_json_files(char *path) {
	struct dirent *direntPtr;
	DIR *dir = opendir(path);
	int count = 0;
	char *subdir;
	
	if (dir  ==  NULL) {
		perror("count_json_files: ");
		return -1;
	}
	while((direntPtr = readdir(dir)) != NULL) {
		if (strcmp(direntPtr->d_name, ".") != 0 && strcmp(direntPtr->d_name, "..") != 0) {
			/* Check if the pointer is a directory, and visit recursively */
			if (direntPtr->d_type  ==  DT_DIR) {
				subdir = calloc(strlen(path) + 2 + strlen(direntPtr->d_name),sizeof(char));
				
				/* Construct the path to the subdirectory */
				strcat(subdir,path);
				if (path[strlen(path)-1] != '/')
					strcat(subdir,"/");
				strcat(subdir,direntPtr->d_name);
				
				/* Recurse into the subdirectory */
				count += count_json_files(subdir);
				free(subdir);
			}
			/* Check if the pointer is a regular json file */
			else if (direntPtr->d_type  ==  DT_REG && (strcmp(strrchr(direntPtr->d_name,'.'), ".json")  ==  0)) {
				count++;
			}
		}
	}
	closedir(dir);
	return count;
}

int pick_the_buckets(int argc,char **argv)
{
	int number_of_buckets;
	long ret;
	char *ptr;
	FILE *fp;
	if (argc == 3)
	{
		if ((fp = fopen(argv[2],"r")) == NULL) {
			printf("Argument error: File '%s' doesn't exist\n", argv[2]);
			return -1;
		}
		fclose(fp);
		number_of_buckets = count_json_files(argv[1]);
		if (number_of_buckets <= 0)
		{
			printf("No data to be input\n");
			return -1;
		}
	}
	else if (argc == 5)
	{
		if ((fp = fopen(argv[2],"r")) == NULL) {
			printf("Argument error: File '%s' doesn't exist\n", argv[2]);
			return -1;
		}
		fclose(fp);
		if (strcmp(argv[3],"-s")  ==  0)	/* there is -s argument */
		{
			ret = strtol(argv[4], &ptr, 10);
			if (ret  ==  0)
			{
				printf("Argument error: false 'size' argument or zero\n");
				return -1;
			}
			number_of_buckets = atoi(argv[4]);
			if (number_of_buckets <= 0)
			{
				printf("Argument error : 'size' argument is negative\n");
				return -1;
			}
			return number_of_buckets; 
		}
		else
		{
			printf("No -s provided. Correct usage: ./run <path to data> <relations csv file> [-s <n>]\n");
			return -1;
		}
	}
	else
	{
		printf("Correct usage: ./run <path to data> <relations csv file> [-s <n>]\n");
		return -1;
	}
	return number_of_buckets;
}


int print_results(struct hash_map *map) {
	int counter=0,relations=0;
	struct vector *vec = NULL;
	struct map_node *ptr;
	struct clique **last;
	struct product *iteration_first_product;
	struct product *print_product;
	int result=0;

	for (int i = 0; i < map->size; ++i)
	{
		ptr = map->array[i];

		while (ptr != NULL) //there are things to see
		{
			if (counter == 0)
			{
				vec = vector_init(1, NULL);
				vector_push_back(vec, *(struct clique**) map->array[i]->value);
				counter++;
				last = (struct clique **)ptr->value;
				iteration_first_product = (*last)->first_product;
				while (iteration_first_product != NULL)
				{
					print_product = iteration_first_product->next;
					while (print_product != NULL)
					{
						relations++;
						printf("%s//%d %s//%d \n", iteration_first_product->website, iteration_first_product->id,print_product->website,print_product->id );
						print_product = print_product->next;
					}
					iteration_first_product = iteration_first_product->next;
				}
			}
			else
				result = vector_search_clique(vec, *(struct clique**) ptr->value); //checks if the clique you want to print has already been printed 

			if (result == -1) 
			{
				// you can print this clique
				last = (struct clique **)ptr->value;
				iteration_first_product = (*last)->first_product;
				while (iteration_first_product != NULL)
				{
					print_product = iteration_first_product->next;
					while (print_product != NULL)
					{
						relations++;
						printf("%s//%d %s//%d \n",iteration_first_product->website, iteration_first_product->id, print_product->website,print_product->id );
						print_product = print_product->next;
					}
					iteration_first_product = iteration_first_product->next;
				}
				vector_push_back(vec, *(struct clique**) last);
			}
		   ptr = ptr->next;      //check the next bucket
		}	
	} 
	vector_delete(vec);  
	return relations;

}

int print_negative_results(struct hash_map *map) {
	int counter = 0, relations = 0;
	struct vector *vec = NULL;
	struct map_node *ptr;
	struct clique **last;
	struct product *iteration_first_product;
	int result=0;

	for (int i = 0; i < map->size; ++i)
	{
		ptr = map->array[i];

		while (ptr != NULL) //there are things to see
		{
			if (counter == 0)
			{
				vec = vector_init(1, NULL);

				vector_push_back(vec, *(struct clique**) map->array[i]->value);
				counter++;
				last = (struct clique **)ptr->value;
				iteration_first_product = (*last)->first_product;

				printf("The clique consists of:\n");
				
				while (iteration_first_product!=NULL)
				{
					printf("%s//%d \n",iteration_first_product->website, iteration_first_product->id);
					iteration_first_product = iteration_first_product->next;
				}

				printf("The negative has the following members: \n");

				struct negative_relation *list_ptr = (*last)->first_negative;

				while (list_ptr != NULL)
				{
					iteration_first_product = list_ptr->neg_rel->first_product;

					while (iteration_first_product != NULL)
					{
						printf("%s//%d \n",iteration_first_product->website, iteration_first_product->id);
						iteration_first_product = iteration_first_product->next;
					}

					list_ptr=list_ptr->next;
				}

			}
			else
				result = vector_search_clique(vec, *(struct clique**) ptr->value); //checks if the clique you want to print has already been printed 

			if (result == -1) 
			{
				// you can print this clique
				last = (struct clique **)ptr->value;
				
				iteration_first_product = (*last)->first_product;

				printf("The clique consists of:\n");
				
				while (iteration_first_product != NULL)
				{
					printf("%s//%d \n",iteration_first_product->website, iteration_first_product->id);
					iteration_first_product=iteration_first_product->next;
				}

				printf("The negative has the following members: \n");

				struct negative_relation *list_ptr = (*last)->first_negative;

				while (list_ptr != NULL)
				{
					iteration_first_product = list_ptr->neg_rel->first_product;

					while (iteration_first_product != NULL)
					{
						printf("%s//%d \n", iteration_first_product->website, iteration_first_product->id);
						iteration_first_product = iteration_first_product->next;
					}
					printf("end\n");
					list_ptr = list_ptr->next;
				}
				
				vector_push_back(vec,*(struct clique**) last);  
			}
		   ptr = ptr->next;      //check the next bucket
		}	
	} 
	vector_delete(vec);  
	return relations;

}