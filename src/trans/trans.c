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
    for (int i = 0; i < len; i++) {
        // 生成一个 N(0,1) 的正态分布随机数
        double U1 = ((double)rand() + 1.0) / ((double)RAND_MAX + 1.0);
        double U2 = ((double)rand() + 1.0) / ((double)RAND_MAX + 1.0);
        double Z = sqrt(-2.0 * log(U1)) * cos(2.0 * M_PI * U2);  // 只取一个

        // 添加高斯噪声
        y[i] = d[i] + sigma * Z;
    }
}

// 译码端接收函数
void Receiver_LLR(double* y, double* LLR_y, int len, double sigma) {
    double sigma_sq = sigma * sigma;
    for(int i = 0; i < len; i++) {
        LLR_y[i] = 2.0 * y[i] / sigma_sq;
    }
}