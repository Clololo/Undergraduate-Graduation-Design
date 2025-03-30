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

// AWGN信道函数
void AWGN_Channel(double* d, double* y, int len, double SNR_dB, double R) {
    // 计算SNR和sigma
    double SNR_linear = pow(10.0, SNR_dB / 10.0);
    double sigma = sqrt(1.0 / SNR_linear);
    
    // 添加高斯噪声
    for(int i = 0; i < len; i++) {
        double noise = sigma * sqrt(-2.0 * log((double)rand() / RAND_MAX)) * 
                      cos(2.0 * M_PI * (double)rand() / RAND_MAX);
        y[i] = d[i] + noise;
    }
}

// 译码端接收函数
void Receiver_LLR(double* y, double* LLR_y, int len, double sigma) {
    double sigma_sq = sigma * sigma;
    for(int i = 0; i < len; i++) {
        LLR_y[i] = 2.0 * y[i] / sigma_sq;
    }
}