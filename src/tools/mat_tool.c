#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include "../config.h"

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
void swap_rows(int ***matrix, int row1, int row2, int n) {
    for (int i = 0; i < n; i++) {
        int temp = (*matrix)[row1][i];
        (*matrix)[row1][i] = (*matrix)[row2][i];
        (*matrix)[row2][i] = temp;
    }
}

// 交换两列
void swap_columns(int ***matrix, int col1, int col2, int m) {
    for (int i = 0; i < m; i++) {
        int temp = (*matrix)[i][col1];
        (*matrix)[i][col1] = (*matrix)[i][col2];
        (*matrix)[i][col2] = temp;
    }
}



// 求方阵的逆矩阵
bool matrix_inverse(int **A, int **inverse, int n) {
    int i, j, k;

    // 构造增广矩阵 [A | I]
    int **aug = (int **)malloc(n * sizeof(int *));
    for (i = 0; i < n; i++) {
        aug[i] = (int *)malloc(2 * n * sizeof(int));
        for (j = 0; j < n; j++) {
            aug[i][j] = A[i][j];            // 左边是 A
            aug[i][j + n] = (i == j) ? 1 : 0; // 右边是单位矩阵 I
        }
    }

    // 高斯-约旦消元（在 GF(2) 下）
    for (i = 0; i < n; i++) {
        // 选择主元
        if (aug[i][i] == 0) {
            for (k = i + 1; k < n; k++) {
                if (aug[k][i] == 1) {
                    swap_rows(&aug, i, k, 2 * n);
                    break;
                }
            }
        }

        // 如果仍然是 0，则矩阵不可逆
        if (aug[i][i] == 0) {
            for (i = 0; i < n; i++) free(aug[i]);
            free(aug);
            return false;
        }

        // 对其他行进行消元
        for (k = 0; k < n; k++) {
            if (k != i && aug[k][i] == 1) {  // 如果这一行的第 i 列是 1
                for (j = 0; j < 2 * n; j++) {
                    aug[k][j] ^= aug[i][j];  // 按位异或
                }
            }
        }
    }

    // 提取右半部分作为逆矩阵
    for (i = 0; i < n; i++) {
        for (j = 0; j < n; j++) {
            inverse[i][j] = aug[i][j + n];
        }
        free(aug[i]);
    }
    free(aug);
    return true;
}


// 判断矩阵是否为满秩矩阵(GF2规则下)
int is_full_rank(int **matrix, int m, int n) {
    int rank = 0;
    
    // 创建矩阵副本，避免修改原矩阵
    int **mat = (int **)malloc(m * sizeof(int *));
    for (int i = 0; i < m; i++) {
        mat[i] = (int *)malloc(n * sizeof(int));
        for (int j = 0; j < n; j++) {
            mat[i][j] = matrix[i][j];
        }
    }

    // 进行高斯消元
    for (int col = 0; col < n && rank < m; col++) {
        int pivot_row = rank;
        
        // 找到当前列第一个 1 的行
        while (pivot_row < m && mat[pivot_row][col] == 0) {
            pivot_row++;
        }
        
        if (pivot_row == m) continue;  // 如果当前列全是 0，跳过
        
        // 交换当前行与主元行
        if (pivot_row != rank) {
            swap_rows(&mat, pivot_row, rank, n);
        }
        
        // 用主元行消去下面所有行的当前列
        for (int i = 0; i < m; i++) {
            if (i != rank && mat[i][col] == 1) {  // 只对当前列为 1 的行操作
                for (int j = col; j < n; j++) {
                    mat[i][j] ^= mat[rank][j];  // 按位异或
                }
            }
        }

        rank++;
    }

    // 释放内存
    for (int i = 0; i < m; i++) {
        free(mat[i]);
    }
    free(mat);

    return (rank == (m < n ? m : n));  // 若 rank == min(m, n)，则矩阵满秩
}

//调整H使Hp和Hs尽可能满秩
void adjust_H(int **H, int m, int n) {
    for (int col = 0; col < n; col++) {
        for (int swap_col = col + 1; swap_col < n; swap_col++) {
            swap_columns(&H, col, swap_col, m);

            int **Hp;
            int **Hs;
            Hp = (int **)malloc(m * sizeof(int *));
            for (int i = 0; i < m; i++) {
                Hp[i] = (int *)malloc(m * sizeof(int));
            }

            Hs = (int **)malloc(m * sizeof(int *));
            for (int i = 0; i < m; i++) {
                Hs[i] = (int *)malloc((n - m) * sizeof(int));
            }

            // 分割 Hp 和 Hs
            for (int i = 0; i < m; i++) {
                for (int j = 0; j < m; j++) Hp[i][j] = H[i][j];
                for (int j = m; j < n; j++) Hs[i][j - m] = H[i][j];
            }
            
            if (is_full_rank(Hp, m, m) && is_full_rank(Hs, m, n - m)) {
                return;
            }

            swap_columns(&H, col, swap_col, m); // 还原
            free(Hp);
            free(Hs);
        }
    }
}

// 矩阵转置
void transposeMatrix(int **A, int **B, int rows, int cols) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            B[j][i] = A[i][j];
        }
    }
}

// 判断两个矩阵是否互为逆矩阵
int are_inverse_matrices(int **A, int **B, int n) {
    int identity[codelength][codelength] = {0};
    for (int i = 0; i < n; i++) identity[i][i] = 1;
    
    int product[codelength][codelength] = {0};
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            for (int k = 0; k < n; k++) {
                product[i][j] ^= (A[i][k] & B[k][j]);
            }
            if (product[i][j] != identity[i][j]) return 0;
        }
    }
    return 1;
}

//检查 ABT == CDT
int check_matrix_equality(int *w, int *A, int **B, int *C, int **D, int m, int n) {
    int *ABT = (int *)malloc(m * sizeof(int));
    int *CDT = (int *)malloc(n * sizeof(int));

    for(int i = 0; i < m; i++){
        ABT[i] = 0;
        CDT[i] = 0;
    }

    for (int i = 0; i < m; i++) {
        for (int j = 0; j < m; j++) {
            ABT[i] ^= (A[j] & B[i][j]);    
        } 
    }

    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n - m; j++) {
            int tmp = CDT[i];
            CDT[i] ^= (C[j] & D[i][j]);  
        } 
    }

    for (int i = 0; i < m; i++) {
        if (ABT[i] != w[i]) {
            free(CDT);
            free(ABT);
            return 0;
        }
    }
    free(CDT);
    free(ABT);
    return 1;
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