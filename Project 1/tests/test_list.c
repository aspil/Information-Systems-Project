#include "acutest.h"

#include "../include/list.h"

void test_init(void) {
	struct list *list = list_init(NULL, NULL);

	TEST_ASSERT(list != NULL);
	TEST_ASSERT(list_size(list) == 0);

	TEST_ASSERT(list_first(list) == NULL);
	TEST_ASSERT(list_last(list) == NULL);

	list_delete(list);
}

void test_append(void) {
	struct list *list = list_init(NULL, NULL);
	
	// Θα προσθέτουμε, μέσω της insert, δείκτες ως προς τα στοιχεία του π΄ίνακα
	int N = 500;
	int *array = malloc(N * sizeof(*array));					

	for (int i = 0; i < N; i++) {
		// LIST_BOF για εισαγωγή στην αρχή
		list_append(list, &array[i]);
		
		/* Check if the size of the list is incremented */
		TEST_ASSERT(list_size(list) == (i + 1));	

		/* Check if the list's last node has value equal to the one we inserted */								
		TEST_ASSERT(list_last_value(list) == &array[i]);	
	}
	list_delete(list);
	free(array);
}

// int compare_int(void *a, void *b) {
// 	return *(int*)a - *(int*)b;
// }

void test_find(void) {
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

TEST_LIST = {
	{ "list_init", test_init },
	{ "list_append", test_append },
	{ "list_find", test_find },
	
	{ NULL, NULL }
};