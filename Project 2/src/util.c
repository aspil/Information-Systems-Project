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
#include "../include/vectorizer.h"

void shuffle_string_array(char* array[], int size) {
	for (int i = 0; i < size; i++) {
		int new_pos = i + rand() / (RAND_MAX / (size - i) + 1);
		char* temp = array[new_pos];
		array[new_pos] = array[i];
		array[i] = temp;
	}
}

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
	while (str[i] != '\n' && str[i] != '\0')
		i++;

	str[i] = '\0';
	return;
}

void skip_whitespace(char *str) {
	int i, x; // code snippet to ignore whitespaces
	for(i = x = 0; str[i]; ++i) {
		if(!isspace(str[i]) || (i > 0 && !isspace(str[i-1])))
			str[x++] = str[i];
	}
	str[x] = '\0';
	// return str;
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

int pick_the_buckets_and_features(int argc,char **argv,int* features)
{
        int number_of_buckets,max_features,flag=0,flag_2=0;
        long ret;
        char *ptr;
        FILE *fp;
        int counter=0;

        if ((fp = fopen(argv[2],"r")) == NULL)
        {
                fprintf(stderr, "Argument error: File '%s' doesn't exist\n", argv[2]);
                return -1;
        }
		fclose(fp);
        if (argc >3)
        {
                counter=3;
                while (counter<=argc-1)
                {
                        if (counter==argc-1)
                        {
                                printf("Invalid usage.You must provide -s or -f and then a number\n");
                                return -1;
                        }
                        if (strcmp(argv[counter],"-s")==0)
                        {
                                flag=1;
                                counter++;

                                ret = strtol(argv[counter], &ptr, 10);

                                if (ret  ==  0)
                                {
                                        fprintf(stderr, "Argument error: false 'size' argument or zero for hash size\n");
                                        return -1;
                                }
                                number_of_buckets = atoi(argv[counter]);
                                if (number_of_buckets <= 0)
                                {
                                        fprintf(stderr, "Argument error : 'size' argument is negative for hash size\n");
                                        return -1;
                                }
                        }
                        else if (strcmp(argv[counter],"-f")==0)
                        {
                                counter++;
                                flag_2=1;
                                ret = strtol(argv[counter], &ptr, 10);
                                if (ret  ==  0)
                                {
                                        fprintf(stderr, "Argument error: false 'size' argument or zero for max features\n");
                                        return -1;
                                }
                                max_features = atoi(argv[counter]);
                                if (max_features <= 0)
                                {
                                        fprintf(stderr, "Argument error : 'size' argument is negative for max features \n");
                                        return -1;
                                }
                                *features=max_features;
                        }
                        else
                        {
                                printf("Error: Wrong parameter . You should give either -f or -s \n");
                                return -1;
                        }
                        counter++;
                }
        }
        if (flag==0)
        {
                number_of_buckets = count_json_files(argv[1]);

                if (number_of_buckets <= 0)
                {
                        fprintf(stderr, "No data to be input\n");
                        return -1;
                }
        }
        if (flag_2==0)
        {
                printf("You didnt give any max features so i used 5000. If you want to use your own. Use [-f number] at command line \n");
                *features=5000;
        }
        return number_of_buckets;
}

char* preprocess_text(char *str) {
	str = strrem(str,"\\u00d7");
	str = strrem(str,"\\u00b0");
	str = strrem(str,"\\u00e2");
	str = strrem(str,"\\u0080");
	str = strrem(str,"\\u0099");
	str = strrem(str,"\\u201d");
	str = strrem(str,"\\u03a6");
	str = strrem(str,"\'s");

	for (size_t i = 0; i < strlen(str); ++i) {
		
		if ((str[i] == '(') || (str[i] == ')') || (str[i] == ':')
		 || (str[i] == '[') || (str[i] == ']') || (str[i] == '\\')
		 || (str[i] == '+') || (strcmp((str+i), "\\n") == 0)
		 || (str[i] == ',') || (strcmp((str+i), "n/a")==0))
		{
			str[i] = ' ';
		}
		str[i] = tolower(str[i]);
	}
	if (str[strlen(str)-1] == '.')
		str[strlen(str)-1] = '\0';
	return str;
}

char *strrem(char *str, const char *sub) {
    size_t len = strlen(sub);
    if (len > 0) {
        char *p = str;
        while ((p = strstr(p, sub)) != NULL) {
            memmove(p, p + len, strlen(p + len) + 1);
        }
    }
    return str;
}

char* extract_spec_title(char *str) {
	str = str + 1;
	/* Proceed to read "page title" string */
	if (str[0] == '<')
		str = str + 1;
	
	/* We got rid of the first symbols */
	char *helping_str = str;

	while (helping_str[0] != '"') 
	{
		if (helping_str[0] == '>') /* End of page title */
			break;
		
		else if (helping_str[0] == '\\') /* Skip " symbol inside the title */
			if (helping_str[1] == '"')
				helping_str++; 

		helping_str++;
	}
	helping_str[0] = '\0';
	
	/* Get the name of a specification */
	char *spec_title = malloc(strlen(str) + 1);
	strcpy(spec_title,str);
	return spec_title;
}

char* extract_spec_value(char *str, FILE* fp) {
	char *spec_val = NULL, *helping_str;
	/* Find its value by passing these characters <whitespaces,:> */
	str = strlen(str) + 2 + str;
	while (str[0] != '"' && str[0] != '[') 
		str = str + 1;
	
	if (str[0] == '"') {
		str = str + 1;  /* Skip the " symbol */
		
		helping_str = str;
		
		while (helping_str[0] != '"') {
			if (helping_str[0] == '\\')  /* Skip " symbol inside the value */
				if (helping_str[1] == '"')
					helping_str++;

			helping_str++;
		}
		
		helping_str[0] = '\0';
		
		spec_val = malloc(strlen(str) + 1);	// Gets free'd by vector_delete
		strcpy(spec_val,str);
	}
	else if (str[0] == '[') /* There is a list of values that we need to store */
	{
		int counter = 0;
		str = str + 1;
		size_t len = 0;
		ssize_t chars;
		char *line = NULL;
		char *line_ptr = NULL;
		while ((chars = getline(&line, &len, fp)) != -1) {
			line_ptr = line;

			skip_whitespace(line_ptr);

			if (line_ptr[0] == '"') {
				line_ptr = line_ptr + 1;

				helping_str = line_ptr;
				while (helping_str[0] != '"') {
					if (helping_str[0] == '\\')
						if (helping_str[1] == '"') // surpass this "
							helping_str++;

					helping_str++;
				}
				helping_str[0] = '\0';
				counter++;

				if (counter == 1) {
					spec_val = malloc(strlen(line_ptr) + 1);
					strcpy(spec_val,line_ptr);
					
				}
				else {
					spec_val = realloc(spec_val, strlen(spec_val) + strlen(line_ptr) + 2);	// Gets free'd by vector_delete
					strcat(strcat(spec_val, " "), line_ptr);
				}
			}
			else
				break;
		}
		free(line);
	}
	return spec_val;
}

int print_results(struct hash_map *map) {
	int counter=0,relations=0;
	struct vector *vec = NULL;
	struct map_node *ptr;
	struct clique **last;
	struct product *iteration_first_product;
	struct product *print_product;
	int result=0;

	for (unsigned int i = 0; i < map->size; ++i)
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

	for (unsigned int i = 0; i < map->size; ++i)
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

int make_the_files(struct hash_map *map)
{
	int counter = 0;
	// int relations = 0;
	struct vector *vec = NULL;
	struct map_node *ptr;
	struct clique **last;
	struct product *iteration_first_product;
	int result = 0;
	FILE *fptr_1, *fptr_2;

	char string_1[] = "Datasets/positive_relations.csv";
	char string_2[] = "Datasets/negative_relations.csv";

	fptr_1 = fopen(string_1, "w");

	fptr_2 = fopen(string_2, "w");

	fclose(fptr_1);
	fclose(fptr_2);

	for (unsigned int i = 0; i < map->size; ++i)
	{
		ptr = map->array[i];
		while (ptr != NULL) //there are things to see
		{
			if (counter == 0)
			{
				vec = vector_init(1, NULL);

				last = (struct clique**)map->array[i]->value;

				vector_push_back(vec, *last);

				counter++;
				last = (struct clique **)ptr->value;
				iteration_first_product = (*last)->first_product;

				// printf("The clique consists of:\n");
				
				while (iteration_first_product!=NULL)
				{
					if ((*last)->size > 1)
					{
						positive_relations_file(string_1,*last);
					}
					iteration_first_product = iteration_first_product->next;
				}
				// printf("The negative has the following members: \n");

				struct negative_relation *list_ptr = (*last)->first_negative;
				while (list_ptr != NULL)
				{
					iteration_first_product = list_ptr->neg_rel->first_product;
					while (iteration_first_product != NULL)
					{
						// printf("%s//%d \n",iteration_first_product->website, iteration_first_product->id);
						iteration_first_product = iteration_first_product->next;
					}
					list_ptr = list_ptr->next;
				}
			}
			else
			{
				// struct clique **sth = (struct clique**) ptr->value;
				last = (struct clique**) ptr->value;
				result = vector_search_clique(vec, *last); //checks if the clique you want to print has already been printed 
			}
			if (result == -1) 
			{
				last = (struct clique **)ptr->value;
				
				iteration_first_product = (*last)->first_product;

				if ((*last)->size > 1)
					positive_relations_file(string_1,*last);
				
				struct negative_relation *list_ptr = (*last)->first_negative;

				if (list_ptr != NULL)
					negative_relations_file(string_2, *last, list_ptr);

				vector_push_back(vec, *(struct clique**) last);  
			}
		   ptr = ptr->next;      //check the next bucket
		}
	}
	vector_delete(vec);
	return 1;
}

void positive_relations_file(char *name_of_file,struct clique *clique_ptr)
{
	char temp[20];
	FILE *ftp;
	if ((ftp = fopen(name_of_file,"a")) == NULL)
		fprintf(stderr, "Couldnt open file\n");

	struct product *iteration_first_product = clique_ptr->first_product, *print_product;
	while (iteration_first_product != NULL)
	{
		print_product = iteration_first_product->next;
		while (print_product != NULL)
		{
			fputs(iteration_first_product->website,ftp);
			fputs("//",ftp);
			sprintf(temp, "%d", iteration_first_product->id);
			fputs(temp,ftp);
			fputs(",",ftp);
			fputs(print_product->website,ftp);
			fputs("//",ftp);
			sprintf(temp, "%d", print_product->id);
			fputs(temp,ftp);
			fputs(",",ftp);
			fputs("1",ftp);
    		fputs("\n", ftp);

			print_product = print_product->next;
		}
		iteration_first_product = iteration_first_product->next;
	}
	fclose(ftp);
}

void negative_relations_file(char *name_of_file,struct clique *clique_ptr,struct negative_relation *ptr)
{
	
	char temp[20];
	FILE *ftp;
	if ((ftp = fopen(name_of_file,"a")) == NULL)
		fprintf(stderr, "Couldnt open file\n");

	if (ftp == NULL)
		fprintf(stderr, "Couldnt open file\n");

	struct product *tranverse = clique_ptr->first_product, *iteration_product;

	struct negative_relation *tranverse_neg = ptr;

	struct clique *iteration;
	while (tranverse != NULL)
	{
		while (tranverse_neg != NULL)
		{
			iteration = tranverse_neg->neg_rel;

			iteration_product = iteration->first_product;

			while (iteration_product != NULL)
			{
				fputs(tranverse->website,ftp);
				// printf("%s %d \n",tranverse->website,tranverse->id );
				fputs("//",ftp);
				sprintf(temp, "%d", tranverse->id);
				fputs(temp,ftp);
				fputs(",",ftp);
				fputs(iteration_product->website,ftp);
				fputs("//",ftp);
				sprintf(temp, "%d", iteration_product->id);
				fputs(temp,ftp);
				fputs(",",ftp);
				fputs("0",ftp);
    			fputs("\n", ftp);
    			iteration_product = iteration_product->next;
			}

			if (tranverse->next == NULL)
			{
				int counter_pos = 0;

				struct clique *ptr_for_clique = tranverse_neg->neg_rel;

				struct negative_relation *previous_ng,*next_ng;
				
				next_ng = ptr_for_clique->first_negative;

				previous_ng = next_ng;
				while (1)
				{
					if (next_ng->neg_rel == clique_ptr)
						break;
					
					counter_pos++;
					previous_ng = next_ng;
					next_ng = next_ng->next;

				}
				if (counter_pos == 0)
				{
					ptr_for_clique->first_negative = next_ng->next;
					if (next_ng->next == NULL)
						ptr_for_clique->last_negative = NULL;

					free(next_ng);
				}
				else if (next_ng->next == NULL)
				{
					ptr_for_clique->last_negative = previous_ng;
					previous_ng->next = NULL;
					free(next_ng);
				}
				else
				{
					previous_ng->next = next_ng->next;
					free(next_ng);
				}
			}
			tranverse_neg = tranverse_neg->next;
		}
		tranverse_neg = ptr;
		tranverse = tranverse->next;
	}
	fclose(ftp);	
}