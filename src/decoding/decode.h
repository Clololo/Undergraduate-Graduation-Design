#ifndef DECODE_H
#define DECODE_H
#include "../encoding/encode.h"

bool MINBP(int *C, double *data, int **resC, Graph* tanner, int MAX_ITERATIONS, float alpha, float beta, int* iterations, double *value);
bool MINBP_NORMAL(int *C, double *data, int **resC, Graph* tanner, int MAX_ITERATIONS, float alpha, float beta, int* iterations);
bool MINBP_TEST_COMPARE(int *C, double *data, int **resC, Graph* tanner, int MAX_ITERATIONS, float alpha, float beta, int* iterations, double* pop);
void LDPCDecoder_NMS_WBP(int** H, double* LLR_y, double alpha, int iterMax, int m, int n, int* v, int *useIter, int *errorNum, bool useWBP, double* weights);

#endif