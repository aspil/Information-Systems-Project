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

// (int (*)(void*, void*))strcmp;
int main(int argc, char *argv[]) {
	// char *x="./Datasets/camera_specs/2013_camera_specs/";
    // char *y="./Datasets/sigmod_large_labelled_dataset.csv";
	// char *y="./tests/datasetY/datasetY2.csv";
	int size;
	if ((size = pick_the_buckets(argc, argv)) <= 0) {
		return -1;
	}
	/* Safe assignments, error checking was done in pick_the_buckets */
	char *data_path = argv[1];	
	char *relations_file = argv[2];
	printf("The size is %d \n",size );

	struct hash_map *map = map_init(size, hash_str, NULL, free, delete_clique); 
	
	read_data_files(map,size,data_path);
	read_relations(map,relations_file);
	int l = print_results(map);
	printf("%d \n",l);

	// /* Free the allocated memory of the hash table */
	// map_delete(map);

	
	return 0;
}
// int main(int argc, char *argv[]) {
// 	// char *x="./Datasets/camera_specs/2013_camera_specs/";
//     // char *y="./Datasets/sigmod_large_labelled_dataset.csv";
// 	// char *y="./tests/datasetY/datasetY2.csv";
// 	char *x="./Datasets/camera_specs/2013_camera_specs";

//     char *y="./Datasets/sigmod_large_labelled_dataset.csv";

// 	//char *x="/home/fanmfh/Desktop/Project_1/camera_specs/example";

// 	// char *y="/home/fanmfh/Desktop/new_folder/Project 1/Project 1/tests/datasetY/datasetY.csv";

// 	int size = pick_the_buckets(argc,argv);

// 	if (size==-1)
// 	{
// 		printf("You forgot the bucket size \n");
// 		return -1;
// 	}
// 	else if (size==-2)
// 	{
// 		printf("You gave name as bucket size or 0 as bucket size\n");
// 		return -1;
		
// 	}
// 	else if (size==-3)
// 	{
// 		return -1;
// 	}
// 	else if (size==0)
// 	{
// 		return -1; 
// 	}
// 	// int size = count_files(argv[1]);
// 	printf("The size is %d \n",size );
	

// 	struct hash_map *map = map_init(size, hash_str, NULL, free, delete_clique); 
	
// 	int variable = read_data_files(map,size,x);

// 	// //read data Y 
// 	// map_print(map);
// 	read_relations(map,y);
// 	// map_print(map);
// 	int l = print_results(map);
// 	printf("%d \n",l);

// 	/* Free the allocated memory of the hash table */
// 	map_delete(map);

	
// 	return 0;
// }