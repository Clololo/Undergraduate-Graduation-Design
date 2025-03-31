#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "trans.h"

// BPSK调制函数
void BPSK_Modulation(int* x, double* d, int len) {
    for(int i = 0; i < len; i++) {
        d[i] = 1.0 - 2.0 * (double)(x[i]);
    }
}

void AWGN_Channel(double* d, double* y, int len, double sigma, double R) {

    // 添加高斯噪声（使用 Box-Muller 变换）
    for (int i = 0; i < len; i += 2) {
        double U1 = ((double)rand() + 1.0) / ((double)RAND_MAX + 1.0); // 避免 log(0)
        double U2 = ((double)rand() + 1.0) / ((double)RAND_MAX + 1.0);
        double Z1 = sqrt(-2.0 * log(U1)) * cos(2.0 * M_PI * U2);
        double Z2 = sqrt(-2.0 * log(U1)) * sin(2.0 * M_PI * U2);

        // 生成噪声
        double noise1 = sigma * Z1;
        double noise2 = sigma * Z2;
        // 赋值
        y[i] = d[i] + noise1;
        if (i + 1 < len) {
            y[i + 1] = d[i + 1] + noise2;
        }
    }
}

// 译码端接收函数
void Receiver_LLR(double* y, double* LLR_y, int len, double sigma) {
    double sigma_sq = sigma * sigma;
    for(int i = 0; i < len; i++) {
        LLR_y[i] = 2.0 * y[i] / sigma_sq;
    }
}