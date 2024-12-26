#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "../tools/math_tools.h"

// ���峣��
#define MAX_ITER 1000
#define PE_THRESHOLD 1e-5
#define SIGMA_INC 1e-3

// SNR��ֵ���㣨ʹ�ø�˹���ƣ�
double iterative_snr_threshold_GA(double sigma, double *Ecn, double *Evn, 
                                double *vn_degree, double *cn_degree, 
                                double *vn_edge_portion, double *cn_edge_portion, 
                                int vn_len, int cn_len) {
    double current_Pe;
    int iter = 1;

    while (iter <= MAX_ITER) {
        if (iter == 1) {
            // �����һ��Ecn��Evn
            for (int k = 0; k < cn_len; k++) {
                Ecn[k] = phi_inverse(1 - pow(1 - phi(2 / (sigma * sigma)), cn_degree[k]));
            }
            // ����Ave_CN
            double Ave_CN = 0.0;
            for (int k = 0; k < cn_len; k++) {
                Ave_CN += cn_edge_portion[k] * Ecn[k];
            }
            // ����Evn
            for (int k = 0; k < vn_len; k++) {
                Evn[k] = 2 / (sigma * sigma) + vn_degree[k] * Ave_CN;
            }
            // �����������Pe
            current_Pe = 0.0;            
            for (int k = 0; k < vn_len; k++) {
                current_Pe += vn_edge_portion[k] * (1 - normcdf(sqrt(Evn[k] / 2)));
            }

            // �ж���������Ƿ�����Ҫ��
            if (current_Pe < PE_THRESHOLD) {
                sigma += SIGMA_INC;
                iter = 1;  // ���õ���
            } else {
                iter++;
            }
        } else {
            // ������������
            double weighted_sum = 0.0;
            for (int k = 0; k < vn_len; k++) {
                weighted_sum += vn_edge_portion[k] * phi(Evn[k]);
            }
            double tmp = 1 - weighted_sum;

            for (int k = 0; k < cn_len; k++) {
                Ecn[k] = phi_inverse(1 - pow(tmp, cn_degree[k]));
            }
            // ����Ave_CN
            double Ave_CN = 0.0;
            for (int k = 0; k < cn_len; k++) {
                Ave_CN += cn_edge_portion[k] * Ecn[k];
            }
            // ����Evn
            for (int k = 0; k < vn_len; k++) {
                Evn[k] = 2 / (sigma * sigma) + vn_degree[k] * Ave_CN;
            }
            // �����������Pe
current_Pe = 0.0;
            for (int k = 0; k < vn_len; k++) {
                current_Pe += vn_edge_portion[k] * (1 - normcdf(sqrt(Evn[k] / 2)));
            }

            // �ж���������Ƿ�����Ҫ��
            if (current_Pe < PE_THRESHOLD) {
                sigma += SIGMA_INC;
                iter = 1;  // ���õ���
            } else {
                iter++;
            }
        }

        // ����ﵽ����������
        if (iter > MAX_ITER) {
            return sigma - SIGMA_INC;
        }
    }
    return sigma;
}


