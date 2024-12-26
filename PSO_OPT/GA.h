// 度分布函数 (ρ 和 λ)
#ifndef GA_H
#define GA_H

// SNR阈值计算（使用高斯近似）
double iterative_snr_threshold_GA(double sigma, double *Ecn, double *Evn, 
                                double *vn_degree, double *cn_degree, 
                                double *vn_edge_portion, double *cn_edge_portion, 
                                int vn_len, int cn_len);

#endif