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
	int count = 0;
	struct dirent *direntPtr;
	char *subdir;
	DIR *dir = opendir(path);
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
			// fclose(fp);
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


void construct_product(struct clique **ptr,char *path,char *id, char *site)
{
	strip_ext(id);

	// initialazation of the product
	struct product *p = product_init(atoi(id), site, (*ptr));
	
	// (*ptr)->first_product = malloc(sizeof(struct product));
	(*ptr)->first_product = p;
	// (*ptr)->last_product = malloc(sizeof(struct product));
	(*ptr)->last_product = p;
	(*ptr)->size += 1; //increase the size by 1 

	// printf("ptr %p\n\n",(*(ptr->first_product))->specs);


	struct vector *vec;
	FILE *fp;
	char *line = NULL,*temp_1=NULL,*temp2=NULL,*temp_3=NULL;
	size_t len = 0;
	ssize_t read;
	char *str;
	fp = fopen (path,"r");
	if (fp  ==  NULL)
		printf ("Cannot open directory '%s'\n", path);

	while ((read = getline(&line, &len, fp)) != -1) 
	{
		// take the line of the file
		str = line;
		if (line[0] != '{' && line[0] != '}') // ignore { and }
		{
			int i, x; // code snippet to ignore whitespaces
			for(i = x = 0; str[i]; ++i)
				if(!isspace(str[i]) || (i > 0 && !isspace(str[i-1])))
				 	str[x++] = str[i];
			str[x] = '\0';

			// format sequence 
			if (str[0] == '"')
			{
				str = str+1;
				if (str[0] == '<')
					str = str+1; // important for the first spec : page title 
			   
				// we got rid of the first symbols

				char *helping_str = str;

				while (helping_str[0] != '"') 
				{
					if (helping_str[0] == '>') // code snipet for page title end
						break;
					
					else if (helping_str[0] == '\\') // code snipet for " that dont declare the end in this case 
						if (helping_str[1] == '"')
							helping_str++; 

					helping_str++;
				}
				helping_str[0] = '\0';
				
				temp_1 = malloc(strlen(str) + 1); // the spec characteristic, free'd in util.c
				strcpy(temp_1,str);

				/* Find its value by passing these characters <whitespaces,:> */
				str = strlen(str) + 2 + str;
				while (str[0] != '"' && str[0] != '[') 
					str = str+1;
				
				if (str[0] == '"')
				{
					str = str+1;  // pass the "
					
					helping_str = str;

					while (helping_str[0] != '"')
					{
						if (helping_str[0] == '\\')
							if (helping_str[1] == '"') // surpass this "
								helping_str++;

						helping_str++;
					}
					helping_str[0] = '\0';
					temp2 = malloc(strlen(str) + 1);	// Gets free'd by vector_delete
					strcpy(temp2,str);
					// we have both values spec and value in temp 
					// we can pass it
					vec = vector_init(1, free);
					vector_push_back(vec, temp2);
					push_specs((*ptr), temp_1, vec);
					vector_delete(vec);	
				}
				else if (str[0] == '[') /* There is a list of values that we need to store */
				{
					int counter = 0;
					str = str+1;

					/* the values  will be one on each next line */

					while ((read = getline(&line, &len, fp)) != -1)
					{
						str = line;
						for(i = x = 0; str[i]; ++i)
							if(!isspace(str[i]) || (i > 0 && !isspace(str[i-1]))) // delete the whitespaces 
								str[x++] = str[i];

						str[x] = '\0';
						
						if (str[0] == '"')
						{
							str = str+1;

							helping_str = str;
							while (helping_str[0] != '"')
							{
								if (helping_str[0] == '\\')
									if (helping_str[1] == '"') // surpass this "
										helping_str++;

								helping_str++;
							}
							helping_str[0] = '\0';
							counter++;

							if (counter == 1)
							{
								// Initialazation of the array
								temp_3 = malloc(strlen(str) + 1);	// Gets free'd by vector_delete
								strcpy(temp_3,str);
								vec = vector_init(1, free);
								vector_push_back(vec, temp_3);
							}
							else
							{
								temp_3 = malloc(strlen(str) + 1);	// Gets free'd by vector_delete
								strcpy(temp_3,str);
								vector_push_back(vec, temp_3);
							}		
						}
						else 
						{
							push_specs((*ptr), temp_1, vec);
							break;
						}
					}
					vector_delete(vec);
				}
				free(temp_1);
			}
		} 
	}
	free(line);
	fclose (fp);
}


int print_results(struct hash_map *map) {
    //printf("mpes kai edw \n");
    //struct clique **clique_array=NULL;
    int counter=0,relations=0;
    struct vector *vec;
    struct map_node *ptr;
    struct clique *last,*print_ptr;
    struct product *iteration_first_product;
    struct product *print_product;
    //printf("the size  1 is %ld \n",sizeof(clique_array) );
    int result=0;

    for (int i = 0; i < map->size; ++i)
    {
        ptr = map->array[i];

        while (ptr!= NULL) //there are things to see
        {
            // print_ptr = (struct clique *) ptr->value;
            if (counter==0)
            {
                vec = vector_init(1, NULL);
				vector_push_back(vec,map->array[i]->value);
				// initArray_2(&dyn_arr,1,map->array[i]->value);
                counter++;
                last=ptr->value;
                iteration_first_product=last->first_product;
                while (iteration_first_product!=NULL)
                {
                    print_product=iteration_first_product->next;
                    while (print_product!=NULL)
                    {
                        relations++;
                        printf("%s//%d %s//%d \n",iteration_first_product->website,iteration_first_product->id,print_product->website,print_product->id );
                        print_product=print_product->next;
                    }
                    iteration_first_product=iteration_first_product->next;
                }
            }
            else
            {
                result = vector_search_clique(vec, ptr->value); //checks if the clique you want to print has already been printed 
            }
            if (result==-1) 
            {
               // you can print this clique

                last=ptr->value;
                iteration_first_product = last->first_product;
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
                //printf("done\n");
				vector_push_back(vec,last);
                // insertArray_2(&dyn_arr,last);   
            }

           ptr=ptr->next;      //check the next bucket
         }
            
        }   
        return relations;

}

