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

void AWGN_Channel(double* d, double* y, int len, double sigma, double R, double* weight) { 

    srand(time(NULL));
    for (int i = 0; i < len; i++) {
        // 生成一个 N(0,1) 的正态分布随机数
        double U1 = ((double)rand() + 1.0) / ((double)RAND_MAX + 1.0);
        double U2 = ((double)rand() + 1.0) / ((double)RAND_MAX + 1.0);
        double Z = sqrt(-2.0 * log(U1)) * cos(2.0 * M_PI * U2);  // 只取一个

        // 添加高斯噪声
        y[i] = d[i] + (weight[i]*sigma) * Z;
    }
}

// 译码端接收函数
void Receiver_LLR(double* y, double* LLR_y, int len, double sigma, double* weight) {
    for(int i = 0; i < len; i++) {
        double sigma_sq = sigma * sigma * weight[i] * weight[i];
        LLR_y[i] = 2.0 * y[i] / sigma_sq;
    }
}

void generate_rand_sequence(int n, double lmin, double lmax, double* sequence) {
    int group_size = 8;
    int group_count = (n + group_size - 1) / group_size;

    srand(45);  // 固定种子，保证相同n时序列稳定

    for (int g = 0; g < group_count; g++) {
        double group[group_size];
        double sum = 0.0;

        // 计算允许的最大sum，确保最后一个值在lmin~lmax范围内
        double max_sum = group_size * 1.0 - lmin;
        double min_sum = group_size * 1.0 - lmax;

        for (int i = 0; i < group_size - 1; i++) {
            // 动态调整生成范围，防止最后一个无法满足
            double remain_min = min_sum - sum;
            double remain_max = max_sum - sum;
            double lower = lmin;
            double upper = lmax;

            // 最后一个要留空间
            if (group_size - 1 - i > 0) {
                lower = fmax(lmin, remain_min);
                upper = fmin(lmax, remain_max);
            }

            double val = lower + (upper - lower) * ((double)rand() / RAND_MAX);
            group[i] = val;
            sum += val;
        }

        // 最后一个严格补偿，确保平均数1
        group[group_size - 1] = group_size * 1.0 - sum;

        // 写入序列
        for (int i = 0; i < group_size; i++) {
            int index = g * group_size + i;
            if (index < n) {
                sequence[index] = group[i];
            }
        }
    }

}