#pragma once

double accuracy_score(int *y_true, int *y_pred, int n);

double precision_score(int *y_true, int *y_pred, int n);

double recall_score(int *y_true, int *y_pred, int n);

double f1_score(int *y_true, int *y_pred, int n);
