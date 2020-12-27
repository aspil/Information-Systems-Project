#pragma once

typedef unsigned int (*HashFunction) (void*);
typedef int (*CompareFunction) (void*, void*);
typedef void (*DeleteKeyFunction) (void*);
typedef void (*DeleteValueFunction) (void*);


struct pair {
	union {
		int a_i;
		double a_d;
		char a_c;
		char *a_s;
	};
	union {
		int b_i;
		double b_d;
		char b_c;
		char *b_s;
	};
	// void *a_i;
	// void *b_i;
};
struct triple {
	int a_i;
	double b_d;
	double c_d;
};