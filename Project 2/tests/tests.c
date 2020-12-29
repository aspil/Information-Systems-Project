#include "../include/acutest.h"			// Απλή βιβλιοθήκη για unit testing

#include "../include/heap.h"

#include "../include/list.h"

#include "../include/map.h"

#include "../include/util.h"

#include "../include/clique.h"

#include "../include/dataset_parsing.h"

void shuffle_array(int* array[], int size) {
	for (int i = 0; i < size; i++) {
		int new_pos = i + rand() / (RAND_MAX / (size - i) + 1);
		int* temp = array[new_pos];
		array[new_pos] = array[i];
		array[i] = temp;
	}
}

/* Create an integer */
int* create_int(int value) {
	int* p = malloc(sizeof(int));
	*p = value;
	return p;
}


// void pick_the_buckets1(void)
// {
//     char **arg_v,*array[5];

//     array[0]="./run";
//     array[1]="./tests/example";
//     array[2]="./tests/datasetY/datasetY.csv";
//     array[3]="-s";
//     array[4]="10";

//     arg_v=array;

//     //i should get 10 as the size of the hashtable

//     int size=pick_the_buckets(5,arg_v);

//     TEST_ASSERT(size == 10);


// }
// void pick_the_buckets2(void)
// {
//     char **arg_v,*array[5];

//     array[0]="./run";
//     array[1]="./tests/example";
//     array[2]="./tests/datasetY/datasetY.csv";
//     array[3]="-s";
//     array[4]="ok";

//     arg_v=array;

//     //i should get 10 as the size of the hashtable

//     int size=pick_the_buckets(5,arg_v);

//     TEST_ASSERT(size == -1);
// }

// void pick_the_buckets3(void)
// {
//     char **arg_v,*array[3];

//     array[0]="./run";
//     array[1]="./tests/example";
//     array[2]="./tests/datasetY/datasetY.csv";

//     arg_v=array;

//     //i should get 10 as the size of the hashtable

//     int size=pick_the_buckets(3,arg_v);

//     TEST_ASSERT(size == 9);
// }

// void read_data_files1(void)
// {
//     struct hash_map *map = map_init(10, hash_str, NULL, NULL, free);

//     char *argv="/random/path"; 

//     int variable=read_data_files(map,10,argv);

//     TEST_ASSERT(variable == -1); //-1 for random paths 
// }

// void read_data_files2(void)
// {
//     struct hash_map *map = map_init(10, hash_str, NULL, NULL, free);

//     char *argv="./tests/read_data_x"; 

//     char *key="buy.net//5411";

//     unsigned int pos = map->hash(key) % map->size;

//     int variable=read_data_files(map,10,argv);

//     TEST_ASSERT(variable == 1); //1 for executing correct path

//     //next test that it was hashed properly 

//     int k=strcmp(map->array[pos]->key,key);

//     TEST_ASSERT(k==0);


// }

// void passing_clique(void)
// {
//     struct hash_map *map = map_init(10, hash_str, NULL, NULL, free);

//     char *argv="./tests/read_data_x"; 

//     char *key="buy.net//5411";

//     unsigned int pos = map->hash(key) % map->size;

//     int variable=read_data_files(map,10,argv);

//     TEST_ASSERT(variable == 1); //1 for executing correct path

//     //next test that it was hashed properly 

//     int k=strcmp(map->array[pos]->key,key);

//     TEST_ASSERT(k==0);

//     //i need to see that the right specs were created 

//     struct clique *ptr= (struct clique *) map->array[pos]->value;

//     TEST_ASSERT(ptr->size==1);

//     struct product *ptr_prod=ptr->first_product;

//     TEST_ASSERT(ptr_prod->id==5411);

//     TEST_ASSERT(strcmp(ptr_prod->website,"buy.net")==0);

//     //first spec
//     struct list *list = ptr_prod->specs;
// 	char *str = "page title";
// 	char *spec = "Kodak PIXPRO AZ251 Bridge Digital Camera - 16MP 25X Optical Zoom HD720p -Black 16.15 Price Comparison at Buy.net";
//     TEST_ASSERT(list_find(list,str) == 0);
// 	struct spec *s1 = (struct spec*)list_get(list,0);
//     TEST_ASSERT(strcmp((char*)s1->value[0],spec) == 0);

//     // second spec
// 	str = "";
// 	spec = "sth";
//     TEST_ASSERT(list_find(list,str) == 0);
// 	s1 = (struct spec*)list_get(list,1);
//     TEST_ASSERT(strcmp((char*)s1->value[0],spec) == 0);

//     //last spec
//     str = "height";
// 	char **specs = malloc(2*sizeof(char*));
// 	specs[0] = "9.7 cm";
// 	specs[1] = "3.8 in.";
//     TEST_ASSERT(list_find(list,str) == 0);

// 	s1 = (struct spec*)list_get(list,2);
//     TEST_ASSERT(strcmp((char*)s1->value[0],specs[0]) == 0);

// 	s1 = (struct spec*)list_get(list,2);
// 	TEST_ASSERT(strcmp((char*)s1->value[1],specs[1]) == 0);
// 	free(specs);
// }

// void read_relations_1(void)
// {
//     char *x="./tests/example";

//     char *y="./tests/datasetY/datasetY.csv";

//     struct hash_map *map = map_init(10, hash_str, NULL, NULL, free);

//     int variable=read_data_files(map,10,x);

//     read_relations(map,y);

//     print_results(map);
// }

void test_vector_init(void) {
	struct vector * vec = vector_init(0, NULL);

	TEST_ASSERT(vec != NULL);
	TEST_ASSERT(vector_size(vec) == 0);

	vector_delete(vec);
}

void test_vector_insert(void) {
	struct vector * vec = vector_init(0, NULL);
	int N = 1000;
	int* array = malloc(N * sizeof(*array));

	/* Insert 5000 integers to test the vector's resizing ability */
	for (int i = 0; i < 1000; i++) {
		vector_push_back(vec, &array[i]);
		TEST_ASSERT(vector_size(vec) == i+1);
		TEST_ASSERT(vector_get(vec, i) == &array[i]);
	}

	/* Try to traverse the vector and get its elements */
	for (int i = 0; i < 1000; i++)
		TEST_ASSERT(vector_get(vec, i) == &array[i]);

	vector_delete(vec);
	free(array);
}

void test_get_set(void) {
	struct vector *vec = vector_init(0, NULL);
	int N = 1000;
	int* array = malloc(N * sizeof(*array));

	/* Insert 1000 NULL pointers */
	for (int i = 0; i < 1000; i++)
		vector_push_back(vec, NULL);

	for (int i = 0; i < 1000; i++) {
		TEST_ASSERT(vector_get(vec, i) == NULL);
		vector_set(vec, i, &array[i]);
		TEST_ASSERT(vector_get(vec, i) == &array[i]);
	}

	vector_delete(vec);
	free(array);
}


void test_heap_init(void) {
	struct heapq *heap = heapq_init(compare_int, NULL);

	TEST_ASSERT(heap != NULL);
	TEST_ASSERT(heapq_size(heap) == 0);

	heapq_delete(heap);

}	

void test_heap_insert(void) {
	struct heapq *heap = heapq_init(compare_int, NULL);
	int N = 1000;
	int* array = malloc(N * sizeof(*array));

	// insert N στοιχεία
	for (int i = 0; i < N; i++) {
		array[i] = i;
		heapq_insert(heap, &array[i]);
		TEST_ASSERT(heapq_size(heap) == i+1);
		TEST_ASSERT(heapq_peek(heap) == &array[i]);
	}

	heapq_delete(heap);
	free(array);
}

void test_heap_remove(void) {
	struct heapq *heap = heapq_init(compare_int, free);

	int N = 10000;
	int** array = malloc(N * sizeof(*array));
	for (int i = 0; i < N; i++)
		array[i] = create_int(i);
	shuffle_array(array, N);

	for (int i = 0; i < N; i++)
		heapq_insert(heap, array[i]);

	for (int i = N-1; i >= 0; i--) {
		int* value = heapq_peek(heap);
		TEST_ASSERT(*value == i);
		TEST_ASSERT(heapq_peek(heap) == value);

		heapq_extract(heap);
		TEST_ASSERT(heapq_size(heap) == i);
		free(value);
	}

	heapq_delete(heap);

	heap = heapq_init(compare_int, NULL);
	heapq_insert(heap, &N);
	TEST_ASSERT(heapq_peek(heap) == &N);
	heapq_extract(heap);
	heapq_delete(heap);
	free(array);
}

void test_list_init(void) {
	struct list *list = list_init(NULL, NULL);

	TEST_ASSERT(list != NULL);
	TEST_ASSERT(list_size(list) == 0);

	TEST_ASSERT(list_first(list) == NULL);
	TEST_ASSERT(list_last(list) == NULL);

	list_delete(list);
}

void test_append(void) {
	struct list *list = list_init(NULL, NULL);
	
	int N = 500;
	int *array = malloc(N * sizeof(*array));					

	for (int i = 0; i < N; i++) {
		list_append(list, &array[i]);
		
		/* Check if the size of the list is incremented */
		TEST_ASSERT(list_size(list) == (unsigned int)(i + 1));	

		/* Check if the list's last node has value equal to the one we inserted */								
		TEST_ASSERT(list_last_value(list) == &array[i]);	
	}
	list_delete(list);
	free(array);
}

void test_list_find(void) {
	struct list *list = list_init(compare_int, NULL);
	int N = 1000;
	int* array = malloc(N * sizeof(*array));					

	/* Insert some values */
	for (int i = 0; i < N; i++) {
		array[i] = i;
		list_append(list, &array[i]);
	}

	/* Search the list for all the elements of the array */
	for (int i = 0; i < N; i++) {
		int* value = list_get(list, i); 
		TEST_ASSERT(value == &array[i]);
	}
	/* Check if -1 exists in the list */
	int not_exists = -1;
	TEST_ASSERT(list_find(list, &not_exists) == 0);

	list_delete(list);
	free(array);
}


void test_map_init(void) {

	struct hash_map *map = map_init(7, NULL, compare_int, NULL, NULL);

	TEST_ASSERT(map != NULL);

	map_delete(map);
}

void insert_and_test(struct hash_map *map, void *key, void *value) {
	map_insert(map, key, value);
	TEST_ASSERT(map_find(map, key) == value);
}

void test_map_insert(void) {

	struct hash_map *map = map_init(57, hash_int, compare_int, NULL, NULL);

	int N = 1000;
	int** key_array = malloc(N * sizeof(*key_array));
	int** value_array = malloc(N * sizeof(*value_array));

	for (int i = 0; i < N; i++) {
		key_array[i] = create_int(i);
	}
	/* Test the correct insertion of the two array values */
	for (int i = 0; i < N; i++) {
		value_array[i] = create_int(i);
		insert_and_test(map, key_array[i], value_array[i]);
	}
	map_delete(map);
	free(key_array);
	free(value_array);
}

void test_map_find() {
	struct hash_map *map = map_init(57, hash_int, compare_int, free, free);

	int N = 500;
	int** key_array = malloc(N * sizeof(*key_array));
	int** value_array = malloc(N * sizeof(*value_array));

	for (int i = 0; i < N; i++) {
		key_array[i] = create_int(i);
		value_array[i] = create_int(i);
		map_insert(map, key_array[i], value_array[i]);

		struct map_node *found = map_find_node(map, key_array[i]);
		void *found_key = found->key;
		void *found_val = found->value;

		/* Test correct insertion */
		TEST_ASSERT(found != NULL); 
		TEST_ASSERT(found_key == key_array[i]);
		TEST_ASSERT(found_val == value_array[i]);
	}	
	/* Search for a non existent key */
	int not_exists = 600;
	TEST_ASSERT(map_find(map, &not_exists) == NULL);
	
	map_delete(map);
	free(key_array);
	free(value_array);
}


TEST_LIST = {
	// { "example1", pick_the_buckets1 },
  	// { "example2", pick_the_buckets2 },
    // { "example3", pick_the_buckets3 },
    // { "example4", read_data_files1 },
    // { "example5", read_data_files2 },
    // { "example6", passing_clique },
    // { "example7", read_relations_1 },

	{ "vector_init", test_vector_init },
	{ "vector_push_back", test_vector_insert },
	{ "vector_get_set", test_get_set },

	{ "heapq_init", test_heap_init },
	{ "heapq_insert", test_heap_insert },
	{ "heapq_extract", test_heap_remove },

	{ "map_init",	 test_map_init },
	{ "map_insert",	 test_map_insert },
	{ "map_find", 	 test_map_find },

	{ "list_init", test_list_init },
	{ "list_append", test_append },
	{ "list_find", test_list_find },
	{ NULL, NULL }
};