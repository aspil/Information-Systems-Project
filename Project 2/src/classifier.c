#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include "../include/util.h"
#include "../include/classifier.h"
#include "../include/vectorizer.h"

struct LogisticRegressor* Logistic_Regression_Init() {
	struct LogisticRegressor *model = malloc(sizeof(struct LogisticRegressor));
	return model;
}