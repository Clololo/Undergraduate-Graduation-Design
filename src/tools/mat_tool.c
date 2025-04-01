#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// 定义矩阵乘法函数
void matrixMultiply(int **A, int **B, int ***C, int m, int n, int k) {
    *C = (int **)malloc(m * sizeof(int *));
    for (int i = 0; i < m; i++) {
        (*C)[i] = (int *)malloc(k * sizeof(int));
    }

    for (int i = 0; i < m; i++) {
        for (int j = 0; j < k; j++) {
            (*C)[i][j] = 0;
            for (int l = 0; l < n; l++) {
                (*C)[i][j] ^= A[i][l] * B[l][j];
            }
        }
    }
}

// 矩阵异或加
void matrixAdd(int **A, int **B, int ***C, int rows, int cols) {
    *C = (int **)malloc(rows * sizeof(int *));
    for (int i = 0; i < rows; i++) {
        (*C)[i] = (int *)malloc(cols * sizeof(int));
    }
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            (*C)[i][j] = A[i][j] ^ B[i][j];
        }
    }
}

// 打印矩阵函数
void printMatrix(int **matrix, int rows, int cols) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            printf("%d", matrix[i][j]);
        }
        printf("\n");
    }
}

// 释放矩阵内存函数
void freeMatrix(int **matrix, int rows) {
    for (int i = 0; i < rows; i++) {
        free(matrix[i]);
    }
    free(matrix);
}


// 交换两行
void swap_rows(int ***matrix, int row1, int row2, int size) {
    for (int i = 0; i < size; i++) {
        int temp = (*matrix)[row1][i];
        (*matrix)[row1][i] = (*matrix)[row2][i];
        (*matrix)[row2][i] = temp;
    }
}

// 求方阵的逆矩阵
bool matrix_inverse(int **matrix, int **inverse, int size) {
    int i, j, k;
    // 初始化逆矩阵为单位矩阵
    for (i = 0; i < size; i++) {
        for (j = 0; j < size; j++) {
            if (i == j)
                inverse[i][j] = 1;
            else
                inverse[i][j] = 0;
        }
    }

    // 扩展原矩阵为增广矩阵
    int **exp = (int **)malloc(size * sizeof(int *));
    for (i = 0; i < size; i++) {
        exp[i] = (int *)malloc(2 * size * sizeof(int));
        for (j = 0; j < size; j++) {
            exp[i][j] = matrix[i][j];
            exp[i][j + size] = inverse[i][j];
        }
    }

    // gauss_jordan
    for (i = 0; i < size; i++) {
        // 选择主元素
        if (exp[i][i] == 0) {
            for (k = i + 1; k < size; k++) {
                if (exp[k][i] != 0) {
                    swap_rows(&exp, i, k, 2 * size);
                    break;
                }
            }
        }

        int pivot = exp[i][i];
        if (pivot == 0) {
            // printf("Matrix is singular and cannot be inverted.\n");
            return false;
        }

        // 归一化主元素所在行
        for (j = 0; j < 2 * size; j++) {
            exp[i][j] /= pivot;
        }

        // 消去主元素所在列的其他元素
        for (k = 0; k < size; k++) {
            if (k != i) {
                int factor = exp[k][i];
                for (j = 0; j < 2 * size; j++) {
                    exp[k][j] ^= factor & exp[i][j];
                }
            }
        }
    }

    // 提取逆矩阵
    for (i = 0; i < size; i++) {
        for (j = 0; j < size; j++) {
            inverse[i][j] = exp[i][j + size];
        }
        free(exp[i]);
    }
    free(exp);
    return true;
}

// 矩阵转置
void transposeMatrix(int **A, int **B, int rows, int cols) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            B[j][i] = A[i][j];
        }
    }
}

void copyPreMat_8_16(char pre[8][16], int ***mat, int m, int n){
    *mat = (int **)malloc(m * sizeof(int *));
    for (int i = 0; i < m; i++) {
        (*mat)[i] = (int *)malloc(n * sizeof(int));
    }
    for(int i = 0; i < m; i++) {
        for(int j = 0; j < n; j++){
            (*mat)[i][j] = pre[i][j] -'0' + 0;
        }
    }
}

void copyPreMat_16_32(char pre[16][32], int ***mat, int m, int n){
    *mat = (int **)malloc(m * sizeof(int *));
    for (int i = 0; i < m; i++) {
        (*mat)[i] = (int *)malloc(n * sizeof(int));
    }
    for(int i = 0; i < m; i++) {
        for(int j = 0; j < n; j++){
            (*mat)[i][j] = pre[i][j] -'0' + 0;
        }
    }
}

void copyPreMat_32_64(char pre[32][64], int ***mat, int m, int n){
    *mat = (int **)malloc(m * sizeof(int *));
    for (int i = 0; i < m; i++) {
        (*mat)[i] = (int *)malloc(n * sizeof(int));
    }
    for(int i = 0; i < m; i++) {
        for(int j = 0; j < n; j++){
            (*mat)[i][j] = pre[i][j] -'0' + 0;
        }
    }
}

void copyPSOMat(const char *filename, int ***H2D, int m, int n) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    // **在函数内动态分配二维数组**
    *H2D = (int **)malloc(m * sizeof(int *));
    if (!*H2D) {
        perror("Memory allocation failed for rows");
        fclose(file);
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < m; i++) {
        (*H2D)[i] = (int *)malloc(n * sizeof(int));
        if (!(*H2D)[i]) {
            perror("Memory allocation failed for columns");
            fclose(file);
            exit(EXIT_FAILURE);
        }
    }

    // **读取文件中的数据填充矩阵**
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            if (fscanf(file, "%d%*c", &((*H2D)[i][j])) != 1) { // %*c 跳过逗号或换行符
                fprintf(stderr, "Error reading matrix at (%d, %d)\n", i, j);
                fclose(file);
                exit(EXIT_FAILURE);
            }
        }
    }

    fclose(file);
}