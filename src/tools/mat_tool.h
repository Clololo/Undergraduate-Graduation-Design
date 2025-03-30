#ifndef MAT_TOOL_H
#define MAT_TOOL_H
#include <stdbool.h>

void freeMatrix(int **matrix, int rows);
void matrixMultiply(int **A, int **B, int ***C, int m, int n, int k);
void matrixAdd(int **A, int **B, int ***C, int rows, int cols);

void printMatrix(int **matrix, int rows, int cols);

void swap_rows(int ***matrix, int row1, int row2, int size);
bool matrix_inverse(int **matrix, int ***inverse, int size);
void transposeMatrix(int **A, int ***B, int rows, int cols);

void copyPreMat_8_16(char pre[8][16], int ***mat, int m, int n);
void copyPreMat_16_32(char pre[16][32], int ***mat, int m, int n);
void copyPreMat_32_64(char pre[32][64], int ***mat, int m, int n);
void copyPSOMat(const char *filename, int ***H2D, int m, int n);

#endif
