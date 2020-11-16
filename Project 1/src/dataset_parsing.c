#include <dirent.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <fcntl.h> // for open
#include <unistd.h>
#include "../include/util.h"
#include "../include/map.h"
#include "../include/clique.h"
#include "../include/dataset_parsing.h"

int read_data_files(struct hash_map *ptr, int size, char *path)
{
	struct dirent *pDirent, *iDirent;
	int counter=0,counter1=0;
	char *subdir, *value = NULL, *path_to_file;
	struct clique *new_clique;
	DIR *pDir, *iDir;

	pDir = opendir(path); //anoigma tou path
	if (pDir == NULL) 
	{
		printf ("Cannot open directory '%s'\n", path);
		return -1;
	}

	while ((pDirent = readdir(pDir)) != NULL) 
	{
		if (pDirent->d_name[0] != '.') //diabase ola ektos apo tis . 
		{
			counter++;
			subdir = malloc (strlen(path)+ 2 + strlen(pDirent->d_name));

			strcpy(subdir, path);
			strcat(subdir, "/"); //create each path for each site
			strcat(subdir, pDirent->d_name);
			iDir = opendir(subdir); 

			if (iDir == NULL) 
			{
				printf("Cannot open directory '%s'\n", subdir);
				return -1;
			}

			while ((iDirent = readdir(iDir)) != NULL) //open site folder
			{
				if (iDirent->d_name[0] != '.') //open each product file
				{
					counter1++;
					char *path_help;
					
					path_help = malloc(strlen(iDirent->d_name) + 3 + strlen(pDirent->d_name));

					strcpy(path_help, pDirent->d_name);

					strcat(path_help,"//");
					
					strcat(path_help,iDirent->d_name);

					strip_ext(path_help); // remove .json and keep the result of site/id as key for hashing

					// create the clique to pass it as argument
					new_clique = create_new();


					// path for file
					path_to_file = malloc(strlen(subdir) + 2 + strlen(iDirent->d_name));
					strcpy(path_to_file, subdir);
					strcat(path_to_file, "/");
					strcat(path_to_file, iDirent->d_name);

					// call the function to create the product and its info
					construct_product(&new_clique, path_to_file, iDirent->d_name, pDirent->d_name);
					free(path_to_file);
					// time for hashing 
					map_insert(ptr, path_help, new_clique);
				}   
			}
			counter1=0;
			closedir(iDir);

			memset(subdir, '\0', strlen(subdir));
			free(subdir);
			subdir = value;
		}
	}
	closedir(pDir);
	return 1;
}

void read_relations(struct hash_map *map, char *path)
{
	// Open the csv file to take relations
	FILE *fp;
	int counter=0;
	char * line = NULL, *temp_1 = NULL, *temp2 = NULL;
	size_t len = 0;
	size_t read;
	char *str;
	fp = fopen(path,"r");
	if (fp == NULL)
		printf("Cannot open directory '%s'\n", path);

	if ((read = getline(&line, &len, fp)) != -1)
	{
		get_line_without_end_line(line);
		if (strcmp(line,"left_spec_id,right_spec_id,label") == 0) //it follows the coding i want
		{
			while ((read = getline(&line, &len, fp)) != -1) 
			{   
				if (line[strlen(line)-2] == '1') //no reason to break into products the 0 values 
				{
					counter++;
					// printf("The line is : %s \n", line);
					char *temp_2=NULL;
					str = line;
					//printf("%s\n",line );


					while (str[0] != ',')
						str = str + 1;

					str[0] = '\0';

					temp_1 = malloc(strlen(line)+1);

					strcpy(temp_1, line); // we got the first product 
					// temp_1 = line;
					temp2 = str + 1 ; // get the second product 

					while (str[0] != ',')
						str = str + 1;

					str[0] = '\0';
					temp_2 = malloc(strlen(temp2)+1);

					strcpy(temp_2,temp2);
					// temp_2 = temp2;
					//we have both now
					// i will hash them to find where they are so i can merge them

					search_and_change(temp_1,temp_2,map);
					free(temp_1);
					free(temp_2);
					temp2=NULL;
				}
					
			}
		}
		else
			printf("Error in coding didnt find the appropriate fields \n");
	}
	free(line);
	fclose(fp);
}