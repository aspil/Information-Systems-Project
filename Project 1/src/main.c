#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <dirent.h>

#include "../include/map.h"
#include "../include/clique.h"
#include "../include/util.h"
int main() {
	int size = 30000;
	//(int (*)(void*, void*))strcmp
	struct hash_map *map = map_init(size, hash_str, NULL, NULL, free);
	
	char *k = "hello";
	struct clique *c = malloc(sizeof(struct clique));
	c->size = 0;
	c->first_product = NULL;
	c->last_product = NULL;
	
	map_insert(map, k, c);
	
	k = "hello2";
	c = malloc(sizeof(struct clique));
	c->size = 0;
	c->first_product = NULL;
	c->last_product = NULL;
	
	map_insert(map, k, c);
	/*--------------------------------------------------------*/

	// struct dirent *pDirent,*iDirent;
	// DIR *pDir,*iDir;
	// char *path = "Datasets/camera_specs/2013_camera_specs";
	// pDir = opendir(path);
	// if (pDir == NULL) {
	// 	printf ("Cannot open directory\n");
	// 	return 1;
	// }

	// // // Process each entry.

	// while ((pDirent = readdir(pDir)) != NULL) {
	// 	if (pDirent->d_name[0] != '.') {
	// 		// puts(pDirent->d_name);
	// 		char *subdir;
	// 		subdir = malloc(sizeof(path)+ 1 + sizeof(pDirent->d_name));
	// 		strcat(strcat(strcat(subdir,path),"/"),pDirent->d_name);
	// 		iDir = opendir(subdir);

	// 		while ((iDirent = readdir(iDir)) != NULL) {
	// 			// iDirent = readdir(iDir);
	// 			if (iDirent->d_name[0] != '.') {
	// 				// printf("pDirent: %s %ld %ld\n", pDirent->d_name, strlen(pDirent->d_name), sizeof(pDirent->d_name));
	// 				// printf("iDirent: %s %ld %ld\n", iDirent->d_name, strlen(iDirent->d_name), sizeof(iDirent->d_name));
	// 				// char *key = malloc(strlen(pDirent->d_name) + strlen(iDirent->d_name)+ 1);
	// 				char *key = malloc(sizeof(pDirent->d_name) + sizeof(iDirent->d_name)+ 2);
	// 				strcat(strcat(strcat(key,pDirent->d_name),"/"),iDirent->d_name);
	// 				strip_ext(key);

	// 				char *file = malloc(sizeof(iDirent->d_name)+ 1);
	// 				strcpy(file,iDirent->d_name);
	// 				strip_ext(file);

	// 				map_insert(map, key, NULL);
	// 				// printf("%s\n",key);
	// 			}
	// 		}
	// 		// printf("%s\n",subdir); 	
	// 	}
	// }
	

	// struct product *p = create_product(147,"ebay.com");
	// char *key1 = produce_key(p->id, p->website);	/* key has size 8!! */

	// struct clique *c = malloc(sizeof(struct clique));
	// // c->first_product = p;
	// // c->last_product = p;
	// struct product *p2 = create_product(147,"eba.com");
	// char *key2 = produce_key(p2->id, p2->website);	/* key has size 8!! */
	// map_insert(map, key1, c);
	// map_insert(map, key2, c);

	/* Free the allocated memory of the hash table */
	map_clear(map);

	return 0;
}