app:
	gcc -c -Iinclude modules/*.c
	gcc -c -Iinclude src/*.c
	gcc *.o -o app

test_app:
	gcc -c -g -Wall -O0 -Iinclude modules/*.c
	gcc -c -g -Wall -O0 -Iinclude src/util.c
	gcc -c -g -Wall -O0 -Iinclude src/clique.c
	gcc -c -g -Wall -O0 -Iinclude src/dataset_parsing.c
	gcc -c -g -Wall -O0 -Iinclude tests/pick_buckets.c

	gcc map.o util.o list.o vector.o dataset_parsing.o clique.o pick_buckets.o -o test

test_map:
	gcc -c -g -Wall -O0 -Iinclude modules/*.c
	gcc -c -g -Wall -O0 -Iinclude src/util.c
	gcc -c -g -Wall -O0 -Iinclude src/clique.c
	gcc -c -g -Wall -O0 -Iinclude src/dataset_parsing.c
	gcc -c -g -Wall -O0 -Iinclude tests/test_map.c

	gcc map.o util.o list.o vector.o dataset_parsing.o clique.o test_map.o -o test

test_vector:
	gcc -c -g -Wall -O0 -Iinclude modules/*.c
	gcc -c -g -Wall -O0 -Iinclude src/util.c
	gcc -c -g -Wall -O0 -Iinclude src/clique.c
	gcc -c -g -Wall -O0 -Iinclude src/dataset_parsing.c
	gcc -c -g -Wall -O0 -Iinclude tests/test_vector.c

	gcc map.o util.o list.o vector.o dataset_parsing.o clique.o test_vector.o -o test

test_list:
	gcc -c -g -Wall -O0 -Iinclude modules/*.c
	gcc -c -g -Wall -O0 -Iinclude src/util.c
	gcc -c -g -Wall -O0 -Iinclude src/clique.c
	gcc -c -g -Wall -O0 -Iinclude src/dataset_parsing.c
	gcc -c -g -Wall -O0 -Iinclude tests/test_list.c

	gcc map.o util.o list.o vector.o dataset_parsing.o clique.o test_list.o -o test

.PHONY:
	clean
	
clean:
	rm -rf *.o app test
