#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "trans.h"

// BPSK���ƺ���
void BPSK_Modulation(int* x, double* d, int len) {
    for(int i = 0; i < len; i++) {
        d[i] = 1.0 - 2.0 * (double)(x[i]);
    }
}

void AWGN_Channel(double* d, double* y, int len, double sigma, double R, double* weight) { 

    srand(time(NULL));
    for (int i = 0; i < len; i++) {
        // ����һ�� N(0,1) ����̬�ֲ������
        double U1 = ((double)rand() + 1.0) / ((double)RAND_MAX + 1.0);
        double U2 = ((double)rand() + 1.0) / ((double)RAND_MAX + 1.0);
        double Z = sqrt(-2.0 * log(U1)) * cos(2.0 * M_PI * U2);  // ֻȡһ��

        // ��Ӹ�˹����
        y[i] = d[i] + (weight[i]*sigma) * Z;
    }
}

// ����˽��պ���
void Receiver_LLR(double* y, double* LLR_y, int len, double sigma, double* weight) {
    for(int i = 0; i < len; i++) {
        double sigma_sq = sigma * sigma * weight[i] * weight[i];
        LLR_y[i] = 2.0 * y[i] / sigma_sq;
    }
}

void generate_rand_sequence(int n, double lmin, double lmax, double* sequence) {
    int group_size = 8;
    int group_count = (n + group_size - 1) / group_size;

    srand(45);  // �̶����ӣ���֤��ͬnʱ�����ȶ�

    for (int g = 0; g < group_count; g++) {
        double group[group_size];
        double sum = 0.0;

        // �����ǰ7������֤���Ϸ�
        for (int i = 0; i < group_size - 1; i++) {
            double val = lmin + (lmax - lmin) * ((double)rand() / RAND_MAX);
            group[i] = val;
            sum += val;
        }

        // �������һ��������Ŀ��ֵ
        group[group_size - 1] = group_size * 1.0 - sum;

        // �������ֵ������Χ���ü��Ϸ�
        if (group[group_size - 1] < lmin) group[group_size - 1] = lmin;
        if (group[group_size - 1] > lmax) group[group_size - 1] = lmax;

        // д������
        for (int i = 0; i < group_size; i++) {
            int index = g * group_size + i;
            if (index < n) {
                sequence[index] = group[i];
            }
        }
    }

}