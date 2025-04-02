#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include "../config.h"

// �������˷�����
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

// ��������
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

// ��ӡ������
void printMatrix(int **matrix, int rows, int cols) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            printf("%d", matrix[i][j]);
        }
        printf("\n");
    }
}

// �ͷž����ڴ溯��
void freeMatrix(int **matrix, int rows) {
    for (int i = 0; i < rows; i++) {
        free(matrix[i]);
    }
    free(matrix);
}

// ��������
void swap_rows(int ***matrix, int row1, int row2, int n) {
    for (int i = 0; i < n; i++) {
        int temp = (*matrix)[row1][i];
        (*matrix)[row1][i] = (*matrix)[row2][i];
        (*matrix)[row2][i] = temp;
    }
}

// ��������
void swap_columns(int ***matrix, int col1, int col2, int m) {
    for (int i = 0; i < m; i++) {
        int temp = (*matrix)[i][col1];
        (*matrix)[i][col1] = (*matrix)[i][col2];
        (*matrix)[i][col2] = temp;
    }
}



// ����������
bool matrix_inverse(int **A, int **inverse, int n) {
    int i, j, k;

    // ����������� [A | I]
    int **aug = (int **)malloc(n * sizeof(int *));
    for (i = 0; i < n; i++) {
        aug[i] = (int *)malloc(2 * n * sizeof(int));
        for (j = 0; j < n; j++) {
            aug[i][j] = A[i][j];            // ����� A
            aug[i][j + n] = (i == j) ? 1 : 0; // �ұ��ǵ�λ���� I
        }
    }

    // ��˹-Լ����Ԫ���� GF(2) �£�
    for (i = 0; i < n; i++) {
        // ѡ����Ԫ
        if (aug[i][i] == 0) {
            for (k = i + 1; k < n; k++) {
                if (aug[k][i] == 1) {
                    swap_rows(&aug, i, k, 2 * n);
                    break;
                }
            }
        }

        // �����Ȼ�� 0������󲻿���
        if (aug[i][i] == 0) {
            for (i = 0; i < n; i++) free(aug[i]);
            free(aug);
            return false;
        }

        // �������н�����Ԫ
        for (k = 0; k < n; k++) {
            if (k != i && aug[k][i] == 1) {  // �����һ�еĵ� i ���� 1
                for (j = 0; j < 2 * n; j++) {
                    aug[k][j] ^= aug[i][j];  // ��λ���
                }
            }
        }
    }

    // ��ȡ�Ұ벿����Ϊ�����
    for (i = 0; i < n; i++) {
        for (j = 0; j < n; j++) {
            inverse[i][j] = aug[i][j + n];
        }
        free(aug[i]);
    }
    free(aug);
    return true;
}


// �жϾ����Ƿ�Ϊ���Ⱦ���(GF2������)
int is_full_rank(int **matrix, int m, int n) {
    int rank = 0;
    
    // �������󸱱��������޸�ԭ����
    int **mat = (int **)malloc(m * sizeof(int *));
    for (int i = 0; i < m; i++) {
        mat[i] = (int *)malloc(n * sizeof(int));
        for (int j = 0; j < n; j++) {
            mat[i][j] = matrix[i][j];
        }
    }

    // ���и�˹��Ԫ
    for (int col = 0; col < n && rank < m; col++) {
        int pivot_row = rank;
        
        // �ҵ���ǰ�е�һ�� 1 ����
        while (pivot_row < m && mat[pivot_row][col] == 0) {
            pivot_row++;
        }
        
        if (pivot_row == m) continue;  // �����ǰ��ȫ�� 0������
        
        // ������ǰ������Ԫ��
        if (pivot_row != rank) {
            swap_rows(&mat, pivot_row, rank, n);
        }
        
        // ����Ԫ����ȥ���������еĵ�ǰ��
        for (int i = 0; i < m; i++) {
            if (i != rank && mat[i][col] == 1) {  // ֻ�Ե�ǰ��Ϊ 1 ���в���
                for (int j = col; j < n; j++) {
                    mat[i][j] ^= mat[rank][j];  // ��λ���
                }
            }
        }

        rank++;
    }

    // �ͷ��ڴ�
    for (int i = 0; i < m; i++) {
        free(mat[i]);
    }
    free(mat);

    return (rank == (m < n ? m : n));  // �� rank == min(m, n)�����������
}

//����HʹHp��Hs����������
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

            // �ָ� Hp �� Hs
            for (int i = 0; i < m; i++) {
                for (int j = 0; j < m; j++) Hp[i][j] = H[i][j];
                for (int j = m; j < n; j++) Hs[i][j - m] = H[i][j];
            }
            
            if (is_full_rank(Hp, m, m) && is_full_rank(Hs, m, n - m)) {
                return;
            }

            swap_columns(&H, col, swap_col, m); // ��ԭ
            free(Hp);
            free(Hs);
        }
    }
}

// ����ת��
void transposeMatrix(int **A, int **B, int rows, int cols) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            B[j][i] = A[i][j];
        }
    }
}

// �ж����������Ƿ�Ϊ�����
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

//��� ABT == CDT
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

    // **�ں����ڶ�̬�����ά����**
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

    // **��ȡ�ļ��е�����������**
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            if (fscanf(file, "%d%*c", &((*H2D)[i][j])) != 1) { // %*c �������Ż��з�
                fprintf(stderr, "Error reading matrix at (%d, %d)\n", i, j);
                fclose(file);
                exit(EXIT_FAILURE);
            }
        }
    }

    fclose(file);
}