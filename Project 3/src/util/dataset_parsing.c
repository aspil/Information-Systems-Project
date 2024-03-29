#include "../../include/util/dataset_parsing.h"

#include "../../include/clique/clique.h"
#include "../../include/util/text_preprocessing.h"
#include "../../include/util/util.h"
#include "../../modules/hashtable/map.h"

#include <ctype.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int read_data_files(struct hash_map *ptr, int size, char *path)
{
	struct dirent * pDirent;
	DIR *			pDir;
	char *			subdir;
	char *			path_to_file;
	struct clique **new_clique;
	pDir = opendir(path);	 // anoigma tou path
	if (pDir == NULL) {
		// printf("Cannot open directory '%s'\n", path);
		return -1;
	}

	while ((pDirent = readdir(pDir)) != NULL) {
		if (strcmp(pDirent->d_name, ".") != 0 &&
			strcmp(pDirent->d_name, "..") != 0)	   // diabase ola ektos apo tis . kai ..
		{
			if (pDirent->d_type == DT_DIR) {
				subdir = calloc(strlen(path) + 2 + strlen(pDirent->d_name), sizeof(char));
				/* Construct the path to the subdirectory */
				strcat(subdir, path);
				if (path[strlen(path) - 1] != '/')
					strcat(subdir, "/");
				strcat(subdir, pDirent->d_name);
				/* Recurse into the subdirectory */
				read_data_files(ptr, size, subdir);
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

				char *path_help = malloc(strlen(website) + 3 + strlen(pDirent->d_name));
				strcpy(path_help, website);
				strcat(path_help, "//");
				strcat(path_help, pDirent->d_name);

				strip_ext(path_help);	 // remove .json and keep the result of site/id as
										 // key for hashing
				/* create the clique to pass it as argument */
				new_clique = create_clique();

				/* path for file */
				path_to_file = malloc(strlen(path) + 2 + strlen(pDirent->d_name));
				strcpy(path_to_file, path);

				if (path[strlen(path_to_file) - 1] != '/')
					strcat(path_to_file, "/");

				strcat(path_to_file, pDirent->d_name);
				/* call the function to create the product and its info */
				clique_set_first_product(new_clique, pDirent->d_name, website);
				/* time for hashing */

				map_insert(ptr, path_help, new_clique);
				product_set_clique(ptr, path_help);
				free(path_to_file);
				free(temp_path);
			}
		}
	}
	closedir(pDir);
	return 1;
}

void read_relations(struct hash_map *map, char *path)
{
	// Open the csv file to take relations
	FILE *	fp;
	int		counter = 0;
	char *	line = NULL, *temp_1 = NULL, *temp2 = NULL;
	size_t	len = 0;
	ssize_t read;
	char *	str;
	fp = fopen(path, "r");
	if (fp == NULL)
		printf("Cannot open directory '%s'\n", path);

	if ((read = getline(&line, &len, fp)) != -1) {
		get_line_without_end_line(line);
		if (strcmp(line, "left_spec_id,right_spec_id,label") == 0)	  // it follows the coding i want
		{
			while ((read = getline(&line, &len, fp)) != -1) {
				if (line[strlen(line) - 2] == '1')	  // no reason to break into products the 0 values
				{
					counter++;
					char *temp_2 = NULL;
					str = line;

					while (str[0] != ',')
						str = str + 1;

					str[0] = '\0';

					temp_1 = malloc(strlen(line) + 1);

					strcpy(temp_1, line);	 // we got the first product
					temp2 = str + 1;		 // get the second product

					while (str[0] != ',')
						str = str + 1;

					str[0] = '\0';
					temp_2 = malloc(strlen(temp2) + 1);

					strcpy(temp_2, temp2);
					// we have both now
					// i will hash them to find where they are so i can merge them

					search_and_change(temp_1, temp_2, map, 1);

					free(temp_1);
					free(temp_2);
					// temp2 = NULL;
				}
				else if (line[strlen(line) - 2] == '0') {
					char *temp_2 = NULL;
					str = line;

					while (str[0] != ',')
						str = str + 1;

					str[0] = '\0';

					temp_1 = malloc(strlen(line) + 1);

					strcpy(temp_1, line);	 // we got the first product
					temp2 = str + 1;		 // get the second product

					while (str[0] != ',')
						str = str + 1;

					str[0] = '\0';
					temp_2 = malloc(strlen(temp2) + 1);

					strcpy(temp_2, temp2);

					search_and_change(temp_1, temp_2, map, 0);

					free(temp_1);
					free(temp_2);
					// temp2 = NULL;
				}
			}
		}
		else
			printf("Parsing error: didnt find the appropriate fields \n");
	}
	free(line);
	fclose(fp);
}

void read_relations2(struct hash_map *map, char *path, struct all_info *rr_ptr)
{
	// Open the csv file to take relations
	FILE *	fp;
	int		counter = 0;
	char *	line = NULL, *temp_1 = NULL, *temp2 = NULL;
	size_t	len = 0;
	ssize_t read;
	char *	str;
	fp = fopen(path, "r");
	if (fp == NULL)
		printf("Cannot open directory '%s'\n", path);

	if ((read = getline(&line, &len, fp)) != -1) {
		get_line_without_end_line(line);
		if (strcmp(line, "left_spec_id,right_spec_id,label") == 0)	  // it follows the coding i want
		{
			while ((read = getline(&line, &len, fp)) != -1) {
				if (line[strlen(line) - 2] == '1')	  // no reason to break into products the 0 values
				{}
				else if (line[strlen(line) - 2] == '0') {
					char *temp_2 = NULL;
					str = line;

					while (str[0] != ',')
						str = str + 1;

					str[0] = '\0';

					temp_1 = malloc(strlen(line) + 1);

					strcpy(temp_1, line);	 // we got the first product
					temp2 = str + 1;		 // get the second product

					while (str[0] != ',')
						str = str + 1;

					str[0] = '\0';
					temp_2 = malloc(strlen(temp2) + 1);

					strcpy(temp_2, temp2);

					search_and_change2(temp_1, temp_2, map, rr_ptr);

					free(temp_1);
					free(temp_2);
					temp2 = NULL;
				}
			}
		}
		else
			printf("Parsing error: didnt find the appropriate fields \n");
	}
	printf("The counter is %d \n", counter);
	free(line);
	fclose(fp);
}