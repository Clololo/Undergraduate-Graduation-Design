#ifndef DECODE_H
#define DECODE_H
#include "../encoding/encode.h"

#define error_punish 100

bool MINBP(int *C, double *data, int **resC, Graph* tanner, int MAX_ITERATIONS, float alpha, float beta, int* iterations, double *value);
bool MINBP_NORMAL(int *C, double *data, int **resC, Graph* tanner, int MAX_ITERATIONS, float alpha, float beta, int* iterations);

#endif