#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include "decode.h"
#include "../config.h"
#include "../encoding/encode.h"

void LDPCDecoder_NMS_WBP(int** H, double* LLR_y, double alpha, int iterMax, 
    int m, int n, int* v, int *useIter, int *errorNum, bool useWBP, double* weights) {
    // ��ʼ������

    double* U0i = (double*)malloc(n * sizeof(double));
    double** Uji = (double**)malloc(m * sizeof(double*));
    double** Vij = (double**)malloc(n * sizeof(double*));
    int* x = (int*)malloc(n * sizeof(int));
    *useIter = error_punish;
    *errorNum = 0;

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
        // printf("the %d-th iteration\n", iter+1);
        // 1. ���±����ڵ㵽У��ڵ����Ϣ (Vij)
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < m; j++) {
                if (H[j][i] == 1) {
                    double sum = U0i[i];
                    for (int k = 0; k < m; k++) {
                        if (H[k][i] == 1 && k != j) {
                        // WBP�㷨Ӧ��Ȩ��
                            sum += (useWBP) ? weights[k] * Uji[k][i] : Uji[k][i];
                        }
                    }
                    Vij[i][j] = sum;
                }
            }
        }

        // 2. ����У��ڵ㵽�����ڵ����Ϣ (Uji)
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

                    // Ӧ��NMS��WBP
                    if (useWBP) {
                        Uji[j][i] = prodSign * minVal * weights[j] * alpha; // WBPʹ�ýڵ��ض�Ȩ��
                    } else {
                        Uji[j][i] = prodSign * minVal * alpha; // NMSʹ��ȫ��alpha
                    }
                }
            }
        }

        // 3. Ӳ�о�
        for (int i = 0; i < n; i++) {
            double sum = U0i[i];
            for (int j = 0; j < m; j++) {
                if (H[j][i] == 1) {
                sum += (useWBP) ? weights[j] * Uji[j][i] : Uji[j][i];
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
            *useIter = iter;
            break; // У��ɹ�����ǰ��ֹ����
        }
    }

    for (int j = 0; j < m; j++) {
        int sum = 0;
        for (int i = 0; i < n; i++) {
            if (H[j][i] == 1) {
                sum += x[i];
            }
        }
        if (sum % 2 != 0) {
            *errorNum += 1;
        }
    }

    // ��ȡ��Ϣλ 
    for (int i = 0; i < n; i++) {
        v[i] = x[i];
    }

    // �ͷ��ڴ�
    free(U0i);
    for (int i = 0; i < m; i++) free(Uji[i]);
    for (int i = 0; i < n; i++) free(Vij[i]);
    free(Uji);
    free(Vij);
    free(x);
}