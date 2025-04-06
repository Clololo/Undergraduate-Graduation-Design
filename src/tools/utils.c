#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include "utils.h"


// Function to generate a random binary string of given length
void generateRandomBinaryString(int length, int ***binaryString) {
       // *C = (int **)malloc(m * sizeof(int *));
    *binaryString = (int**)malloc(sizeof(int));

    (*binaryString)[0] = (int *)malloc(length * sizeof(int));

    for (int i = 0; i < length; i++) {
        (*binaryString)[0][i] = (rand() % 2) ? 1 : 0;
    }
    // Null-terminate the string
}

void printBinaryString(int **binaryString, int length){
    for (int i = 0; i < length; i++) {
        printf("%d",binaryString[0][i]);
    }
    printf("\n");
    return;
}

// 读取二维矩阵的第一行到一维数组的函数
void generateDigit(int **binaryString, int length, int **Row) {
    // 分配一维数组的内存
    *Row = (int*)malloc(length * sizeof(int));

    // 将二维矩阵的第一行复制到一维数组
    for (int i = 0; i < length; i++) {
        (*Row)[i] = binaryString[0][i];
    }
}

void fillArrayWithFirstRow(int **matrix1, int **matrix2, int **matrix3, int **array, int rows1, int rows2, int rows3) {
    int index = 0;    
    *array = (int*)malloc((rows1 + rows2 + rows3) * sizeof(int));

    for (int i = 0; i < rows1; i++) {
        (*array)[index++] = matrix1[0][i];
    }

    for (int i = 0; i < rows2; i++) {
        (*array)[index++] = matrix2[0][i];
    }

    for (int i = 0; i < rows3; i++) {
        (*array)[index++] = matrix3[0][i];
    }
}


void printDigit(int *digit, int length){
    for (int i = 0; i < length; i++) {
        printf("%d",digit[i]);
    }
    printf("\n");
    return;
}


//误码模拟部分
double generateRandomFloat() {
    return ((double)rand() / ((double)RAND_MAX + 1) + (double)rand() / ((double)RAND_MAX + 1) / ((double)RAND_MAX + 1));
}


bool randomcheck(float errorRate){
    float randomValue = generateRandomFloat();
    if(randomValue < errorRate){
        //errorBit ++;  
        return true;
    }
    return false;
}

// 模拟码传递过程中出现位反转的函数
void simulateErrors(int **Digit, int length, float errorRate) {
    for (int i = 0; i < length; i++) {
        if (randomcheck(errorRate)) {
            (*Digit)[i] = ((*Digit)[i] == 0) ? 1 : 0;
        }
    }
}

//检查最终出错的位数
int compareDigit(int *array1, int *array2, int length) {
    int errorBit = 0;
    for (int i = 0; i < length; i++) {
        if (array1[i] != array2[i]) {
            errorBit ++;
        }
    }
    return errorBit;
}

int check_codeword(int **H, int *C, int m, int n) {
    int i, j;
    int valid = 1; // 1 表示码字有效, 0 表示无效

    for (i = 0; i < m; i++) {
        int sum = 0;
        for (j = 0; j < n; j++) {
            sum += H[i][j] * C[j]; // 计算 H * C
        }
        if (sum % 2 != 0) { // 取模 2，检查是否全为 0
            valid = 0;
            break;
        }
    }
    
    return valid;
}

// Function to generate a random binary string of given length
void generateS(int length, int *binaryString) {
    for (int i = 0; i < length; i++) {
        binaryString[i] = (rand() / (double)RAND_MAX) < 0.5 ? 0 : 1;
    }
}