#ifndef DECODE_H
#define DECODE_H
#include "../encoding/encode.h"


bool MINBP(int *C, double *data, int **resC, Graph* tanner, int MAX_ITERATIONS, float alpha, float beta, int* iterations, double *value);

#endif