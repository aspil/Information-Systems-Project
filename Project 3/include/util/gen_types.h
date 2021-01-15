#pragma once

typedef unsigned int (*HashFunction)(void *);
typedef int (*CompareFunction)(void *, void *);
typedef void (*DeleteKeyFunction)(void *);
typedef void (*DeleteValueFunction)(void *);