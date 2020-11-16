#include <stdlib.h>
#include "acutest.h"
#include "../include/map.h"



void test_init(void) {

	struct hash_map *map = map_init(7, NULL, compare_int, NULL, NULL);

	TEST_ASSERT(map != NULL);

	map_delete(map);
}

/* Create an integer */
int* create_int(int value) {
	int* p = malloc(sizeof(int));
	*p = value;
	return p;
}

// Βοηθητική συνάρτηση, κάνει insert και ελέγχει αν έγινε η εισαγωγή
void insert_and_test(struct hash_map *map, void *key, void *value) {
	map_insert(map, key, value);
	TEST_ASSERT(map_find(map, key) == value);
}

void test_insert(void) {

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

void test_find() {
	struct hash_map *map = map_init(57, hash_int, compare_int, NULL, NULL);

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
	{ "map_init", test_init },
	{ "map_insert", test_insert },
	{ "map_find", 	test_find },

	{ NULL, NULL } // τερματίζουμε τη λίστα με NULL
}; 