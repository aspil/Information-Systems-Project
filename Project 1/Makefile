all:
	# gcc -c -g -Wall -O0 -Iinclude modules/map.c
	# gcc -c -g -Wall -O0 -Iinclude src/main.c
	# gcc -c -g -Wall -O0 -Iinclude src/util.c
	# gcc -c -g -Wall -O0 -Iinclude src/clique.c
	# gcc -c -g -Wall -O0 -Iinclude modules/array.c
	# gcc -c -g -Wall -O0 -Iinclude src/dataset_parsing.c
	gcc -c -g -Wuninitialized -Wall -O0 -Iinclude modules/*.c
	gcc -c -g -Wuninitialized -Wall -O0 -Iinclude src/*.c
	gcc *.o -o run -lm 

clean:
	rm *.o run
.PHONY:
	clean