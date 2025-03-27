#ifndef UTILS_H
#define UTILS_H
#include <stdbool.h>

extern int errorBit;
extern int errorBitWithLDPC;

void generateRandomBinaryString(int length, int ***binaryString);
void printBinaryString(int **binaryString, int length);
void generateDigit(int **binaryString, int length, int **Row);
void printDigit(int *digit, int length);
void fillArrayWithFirstRow(int **matrix1, int **matrix2, int **matrix3, int **array, int rows1, int rows2, int rows3);

bool randomcheck(float errorRate);
void simulateErrors(int **Digit, int length, float errorRate);

int compareDigit(int *array1, int *array2, int length);

#endif

