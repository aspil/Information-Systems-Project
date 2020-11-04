all:
	gcc -c -g -Wall -O0 -Iinclude modules/map/map.c
	gcc -c -g -Wall -O0 -Iinclude src/main.c
	gcc map.o main.o -o run -lm 

clean:
	rm *.o run
.PHONY:
	clean