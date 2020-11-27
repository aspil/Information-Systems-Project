#include "../include/acutest.h"
#include "../include/vector.h"

void test_init(void) {
	struct vector * vec = vector_init(0, NULL);

	TEST_ASSERT(vec != NULL);
	TEST_ASSERT(vector_size(vec) == 0);

	vector_delete(vec);
}

void test_insert(void) {
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

TEST_LIST = {
	{ "vector_init", test_init },
	{ "vector_push_back", test_insert },
	{ "vector_get_set", test_get_set },

	{ NULL, NULL }
};