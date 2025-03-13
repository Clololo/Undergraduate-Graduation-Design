#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// 打印矩阵
void print_matrix(int **matrix, int rows, int cols) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            printf("%d ", matrix[i][j]);
        }
        printf("\n");
    }
}

// 行交换函数
void swap_rows(int **matrix, int row1, int row2, int cols) {
    printf("swap %d and %d\n",row1,row2);
    int *temp = matrix[row1];
    matrix[row1] = matrix[row2];
    matrix[row2] = temp;
}


// 高斯消元，将矩阵变换为下三角形式
bool GenerateLowTriMat(int ***matrix, int m, int n,int g) {
    int pivot_row = 0;
    int pivot_col = n - m + g;

    while (pivot_row < m - g && pivot_col < n) {
        if(((*matrix)[pivot_row][pivot_col])==0)
        {
            int max_row = -1;
            for (int i = pivot_row + 1; i < m; i++) {
                if ((*matrix)[i][pivot_col]) {
                    max_row = i;
                    break;
                }
            }
            if (max_row != -1) {
                swap_rows((*matrix), max_row, pivot_row, n);
            }
            else{
                return false;
            }
        }

        pivot_row++;
        pivot_col++;
    }

    int now_row = m - g - 1;
    while(now_row){
        for(int row = now_row-1; row>=0; row--){
            if((*matrix)[row][now_row + n - m + g]){
                for(int i = 0;i < n;i++){
                    (*matrix)[row][i] ^= (*matrix)[now_row][i];
                }
            }
        }
        now_row--;
    }
    return true;
}

// 将矩阵划分为六个部分，并进行高斯消元
bool MatrixDivide(int ***H, int ***A, int ***B, int ***T, int ***C, int ***D, int ***E, int m, int n, int g) {
    // 分配内存给六个部分矩阵
    *A = (int **)malloc((m - g) * sizeof(int *));
    *B = (int **)malloc((m - g) * sizeof(int *));
    *T = (int **)malloc((m - g) * sizeof(int *));
    *C = (int **)malloc(g * sizeof(int *));
    *D = (int **)malloc(g * sizeof(int *));
    *E = (int **)malloc(g * sizeof(int *));
    
    for (int i = 0; i < (m - g); i++) {
        (*A)[i] = (int *)malloc((n - m) * sizeof(int));
        (*B)[i] = (int *)malloc(g * sizeof(int));
        (*T)[i] = (int *)malloc((m - g) * sizeof(int));
    }
    for (int i = 0; i < g; i++) {
        (*C)[i] = (int *)malloc((n - m) * sizeof(int));
        (*D)[i] = (int *)malloc(g * sizeof(int));
        (*E)[i] = (int *)malloc((m - g) * sizeof(int));
    }
    
    printf("Matrix h:\n");
    print_matrix(*H, (m), (n));

    // 对整个 H 矩阵进行高斯消元，使右上角的 (m-g) x (m-g) 部分变为下三角形式
    bool flag = GenerateLowTriMat(&H, m, n, g);
    if(!flag){
        return false;
    }
        // 复制矩阵 H 到 T 矩阵中
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            if (i < (m - g) && j < (n - m)) {
                (*A)[i][j] = (*H)[i][j];
            } else if (i < (m - g) && j >= (n - m) && j < (n - m + g)) {
                (*B)[i][j - (n - m)] = (*H)[i][j];
            } else if (i < (m - g) && j >= (n - m + g) && j < n) {
                (*T)[i][j - (n - m + g)] = (*H)[i][j];
            } else if (i >= (m - g) && i < m && j < (n - m)) {
                (*C)[i - (m - g)][j] = (*H)[i][j];
            } else if (i >= (m - g) && i < m && j >= (n - m) && j < (n - m + g)) {
                (*D)[i - (m - g)][j - (n - m)] = (*H)[i][j];
            } else if (i >= (m - g) && i < m && j >= (n - m + g) && j < n) {
                (*E)[i - (m - g)][j - (n - m + g)] = (*H)[i][j];
            }
        }
    }

    // printf("Matrix h:\n");
    // print_matrix(*H, (m), (n));
    // // 打印结果（示例）
    // printf("Matrix A:\n");
    // print_matrix(*A, (m - g), (n - m));
    // printf("\nMatrix B:\n");
    // print_matrix(*B, (m - g), g);
    // printf("\nMatrix T (upper triangular):\n");
    // print_matrix(*T, (m - g), (m - g));  // 打印近似下三角形式的 T 矩阵
    // printf("\nMatrix C:\n");
    // print_matrix(*C, g, (n - m));
    // printf("\nMatrix D:\n");
    // print_matrix(*D, g, g);
    // printf("\nMatrix E:\n");
    // print_matrix(*E, g, (m - g));
    return true;
}

int main() {
    int m = 5, n = 7, g = 2;  // 示例中的 m, n, g 值
    int **H;

    // 示例矩阵 H
    int H_data[5][7] = {
        {1, 1, 0, 0, 1, 0, 0},
        {0, 1, 1, 0, 1, 0, 1},
        {1, 0, 1, 0, 0, 1, 1},
        {0, 1, 0, 1, 1, 1, 1},
        {0, 1, 0, 0, 0, 1, 1}
    };

    // 分配矩阵 H 的内存并复制数据
    H = (int **)malloc(m * sizeof(int *));
    for (int i = 0; i < m; i++) {
        H[i] = (int *)malloc(n * sizeof(int));
        for (int j = 0; j < n; j++) {
            H[i][j] = H_data[i][j];
        }
    }

    // 调用 MatrixDivide 函数
    int **A, **B, **T, **C, **D, **E;
    MatrixDivide(&H, &A, &B, &T, &C, &D, &E, m, n, g);

    // 释放内存
    for (int i = 0; i < m; i++) {
        free(H[i]);
    }
    free(H);
    for (int i = 0; i < (m - g); i++) {
        free(A[i]);
        free(B[i]);
        free(T[i]);
    }
    free(A);
    free(B);
    free(T);
    for (int i = 0; i < g; i++) {
        free(C[i]);
        free(D[i]);
        free(E[i]);
    }
    free(C);
    free(D);
    free(E);

    return 0;
}
