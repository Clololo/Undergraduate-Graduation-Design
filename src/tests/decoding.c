#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include <stdbool.h>

// LDPC���������� - ��һ����С���㷨(NMS)
// ����˵��:
// H: У����� (m x n)
// LLR_y: �����źŵĳ�ʼLLRֵ (����Ϊn������)
// alpha: NMS�㷨�Ĺ�һ������
// iterMax: ����������
// m: У��ڵ���
// n: �����ڵ���
// v: ��������� (����Ϊn-m������)
void LDPCDecoder_NMS(int** H, double* LLR_y, double alpha, int iterMax, 
                    int m, int n, int* v) {
    // ��ʼ������
    double* U0i = (double*)malloc(n * sizeof(double));
    double** Uji = (double**)malloc(m * sizeof(double*));
    double** Vij = (double**)malloc(n * sizeof(double*));
    int* x = (int*)malloc(n * sizeof(int));
    
    for (int i = 0; i < m; i++) {
        Uji[i] = (double*)malloc(n * sizeof(double));
        for (int j = 0; j < n; j++) {
            Uji[i][j] = 0.0;
        }
    }
    
    for (int i = 0; i < n; i++) {
        Vij[i] = (double*)malloc(m * sizeof(double));
        U0i[i] = LLR_y[i];
        for (int j = 0; j < m; j++) {
            Vij[i][j] = 0.0;
        }
    }

    // ��������
    for (int iter = 0; iter < iterMax; iter++) {
        // printf("the %d-th iteration of NMS\n", iter+1);
        
        // 1. ���±����ڵ㵽У��ڵ����Ϣ (Vij)
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < m; j++) {
                if (H[j][i] == 1) {
                    double sum = U0i[i];
                    for (int k = 0; k < m; k++) {
                        if (H[k][i] == 1 && k != j) {
                            sum += Uji[k][i];
                        }
                    }
                    Vij[i][j] = sum;
                }
            }
        }
        
        // 2. ����У��ڵ㵽�����ڵ����Ϣ (Uji) - ʹ��NMS�㷨
        for (int j = 0; j < m; j++) {
            for (int i = 0; i < n; i++) {
                if (H[j][i] == 1) {
                    // ������ų˻� (�ų���ǰi)
                    double prodSign = 1.0;
                    // ������Сֵ (�ų���ǰi)
                    double minVal = DBL_MAX;
                    
                    for (int k = 0; k < n; k++) {
                        if (H[j][k] == 1 && k != i) {
                            prodSign *= (Vij[k][j] >= 0) ? 1.0 : -1.0;
                            if (fabs(Vij[k][j]) < minVal) {
                                minVal = fabs(Vij[k][j]);
                            }
                        }
                    }
                    
                    Uji[j][i] = prodSign * minVal * alpha;
                }
            }
        }
        
        // 3. Ӳ�о�
        for (int i = 0; i < n; i++) {
            double sum = U0i[i];
            for (int j = 0; j < m; j++) {
                if (H[j][i] == 1) {
                    sum += Uji[j][i];
                }
            }
            x[i] = (sum < 0) ? 1 : 0;
        }
        
        // 4. У���Ƿ���������У�鷽��
        bool valid = true;
        for (int j = 0; j < m; j++) {
            int sum = 0;
            for (int i = 0; i < n; i++) {
                if (H[j][i] == 1) {
                    sum += x[i];
                }
            }
            if (sum % 2 != 0) {
                valid = false;
                break;
            }
        }
        
        if (valid) {
            break; // У��ɹ�����ǰ��ֹ����
        }
    }
    
    // ��ȡ��Ϣλ (�����n-mλ����Ϣλ)
    for (int i = m; i < n; i++) {
        v[i-m] = x[i];
    }
    
    // �ͷ��ڴ�
    free(U0i);
    for (int i = 0; i < m; i++) free(Uji[i]);
    for (int i = 0; i < n; i++) free(Vij[i]);
    free(Uji);
    free(Vij);
    free(x);
}

// ���Ժ���
int main() {
    // ʾ������
    int m = 4;  // У��ڵ���
    int n = 8;  // �����ڵ���
    double alpha = 0.8; // NMS��һ������
    int iterMax = 10; // ����������
    
    // ʾ��У����� (4x8)
    int** H = (int**)malloc(m * sizeof(int*));
    for (int i = 0; i < m; i++) {
        H[i] = (int*)malloc(n * sizeof(int));
    }
    
    // ���H���� (ʾ��)
    int exampleH[4][8] = {
        {1, 1, 1, 1, 0, 0, 0, 0},
        {1, 0, 0, 0, 1, 1, 1, 0},
        {0, 1, 0, 0, 1, 0, 0, 1},
        {0, 0, 1, 0, 0, 1, 0, 1}
    };
    
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            H[i][j] = exampleH[i][j];
        }
    }
    
    // ʾ������LLR (ʵ��Ӧ����Ӧ���ŵ����)
    double LLR_y[8] = {1.2, -0.5, 0.8, -1.5, 0.3, -0.9, 1.1, -0.2};
    
    // �������
    int* decoded = (int*)malloc((n-m) * sizeof(int));
    
    // ���ý�����
    LDPCDecoder_NMS(H, LLR_y, alpha, iterMax, m, n, decoded);
    
    // ��ӡ������
    printf("Decoded message: ");
    for (int i = 0; i < n-m; i++) {
        printf("%d ", decoded[i]);
    }
    printf("\n");
    
    // �ͷ��ڴ�
    for (int i = 0; i < m; i++) free(H[i]);
    free(H);
    free(decoded);
    
    return 0;
}