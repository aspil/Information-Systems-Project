app:
	gcc -c -g -Wall -Iinclude modules/*.c
	gcc -c -g -Wall -Iinclude src/*.c 
	gcc *.o -o app -lm

test_app:
	gcc -c -g -Wall -O0 -Iinclude modules/*.c
	gcc -c -g -Wall -O0 -Iinclude src/util.c
	gcc -c -g -Wall -O0 -Iinclude src/clique.c
	gcc -c -g -Wall -O0 -Iinclude src/dataset_parsing.c
	gcc -c -g -Wall -O0 -Iinclude tests/pick_buckets.c

	gcc map.o util.o list.o vector.o dataset_parsing.o clique.o pick_buckets.o -o test
test:
	gcc -c -g -Wall -O0 -Iinclude modules/heap.c
	gcc -c -g -Wall -O0 -Iinclude modules/vector.c
	gcc -c -g -Wall -O0 -Iinclude modules/map.c
	gcc -c -g -Wall -O0 -Iinclude modules/list.c

	# gcc -c -g -Wall -O0 -Iinclude src/clique.c
	# gcc -c -g -Wall -O0 -Iinclude src/util.c

	gcc -c -g -Wall -O0 -Iinclude tests/tests.c

	gcc heap.o vector.o map.o list.o tests.o -o test

.PHONY:
	clean
	
clean:
	rm -rf *.o app test
