#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "../tools/math_tools.h"

// 定义常量
#define MAX_ITER 1000
#define PE_THRESHOLD 1e-5
#define SIGMA_INC 1e-3

// SNR阈值计算（使用高斯近似）
double iterative_snr_threshold_GA(double sigma, double *Ecn, double *Evn, 
                                double *vn_degree, double *cn_degree, 
                                double *vn_edge_portion, double *cn_edge_portion, 
                                int vn_len, int cn_len) {
    double current_Pe;
    int iter = 1;

    while (iter <= MAX_ITER) {
        if (iter == 1) {
            // 计算第一次Ecn和Evn
            for (int k = 0; k < cn_len; k++) {
                Ecn[k] = phi_inverse(1 - pow(1 - phi(2 / (sigma * sigma)), cn_degree[k]));
            }
            // 计算Ave_CN
            double Ave_CN = 0.0;
            for (int k = 0; k < cn_len; k++) {
                Ave_CN += cn_edge_portion[k] * Ecn[k];
            }
            // 计算Evn
            for (int k = 0; k < vn_len; k++) {
                Evn[k] = 2 / (sigma * sigma) + vn_degree[k] * Ave_CN;
            }
            // 计算误比特率Pe
            current_Pe = 0.0;            
            for (int k = 0; k < vn_len; k++) {
                current_Pe += vn_edge_portion[k] * (1 - normcdf(sqrt(Evn[k] / 2)));
            }

            // 判断误比特率是否满足要求
            if (current_Pe < PE_THRESHOLD) {
                sigma += SIGMA_INC;
                iter = 1;  // 重置迭代
            } else {
                iter++;
            }
        } else {
            // 后续迭代过程
            double weighted_sum = 0.0;
            for (int k = 0; k < vn_len; k++) {
                weighted_sum += vn_edge_portion[k] * phi(Evn[k]);
            }
            double tmp = 1 - weighted_sum;

            for (int k = 0; k < cn_len; k++) {
                Ecn[k] = phi_inverse(1 - pow(tmp, cn_degree[k]));
            }
            // 计算Ave_CN
            double Ave_CN = 0.0;
            for (int k = 0; k < cn_len; k++) {
                Ave_CN += cn_edge_portion[k] * Ecn[k];
            }
            // 计算Evn
            for (int k = 0; k < vn_len; k++) {
                Evn[k] = 2 / (sigma * sigma) + vn_degree[k] * Ave_CN;
            }
            // 计算误比特率Pe
current_Pe = 0.0;
            for (int k = 0; k < vn_len; k++) {
                current_Pe += vn_edge_portion[k] * (1 - normcdf(sqrt(Evn[k] / 2)));
            }

            // 判断误比特率是否满足要求
            if (current_Pe < PE_THRESHOLD) {
                sigma += SIGMA_INC;
                iter = 1;  // 重置迭代
            } else {
                iter++;
            }
        }

        // 如果达到最大迭代次数
        if (iter > MAX_ITER) {
            return sigma - SIGMA_INC;
        }
    }
    return sigma;
}


