#ifndef PSO_H
#define PSO_H

#include "../config.h"

extern double Ecn[pred_cn_l], Evn[pred_vn_l];
extern double vn_degree[dimlimit], cn_degree[dimlimit];
extern double vn_edge_portion[dimlimit], cn_edge_portion[dimlimit];

extern double rho[dimlimit];  // 变量节点的度分布概率
extern double lambda[dimlimit];  // 校验节点的度分布概率
extern double messages[dimlimit];  // 输入消息数组
extern int pred_vn_length;
extern int pred_cn_length;

double compute_code_rate(double rho[], double lambda[]);
double calculate_sigma_shannon(double R);
double func(const double x[], double *Ecn, double *Evn, 
    double *vn_degree, double *cn_degree, 
    double *vn_edge_portion, double *cn_edge_portion, double sum_exp_theta_r, double sum_exp_theta_l);
void initPopVFit(int sizePop, double pop[][dimlimit], double v[][dimlimit], double fitness[], double *Ecn, double *Evn, 
        double *vn_degree, double *cn_degree, double *vn_edge_portion, double *cn_edge_portion);
void getInitBest(int sizePop, const double fitness[], const double pop[][dimlimit],
                 double gbestPop[], double *gbestFitness,
                 double pbestPop[][dimlimit], double pbestFitness[]);
double iterative_snr_threshold(double SIGMA, int key, double *Ecn, 
                            double *Evn, double *vn_degree, double *cn_degree, 
                            double *vn_edge_portion, double *cn_edge_portion, 
                            int vn_len, int cn_len);
void update_particles(int sizePop, double pop[][dimlimit], double v[][dimlimit], double fitness[],
                      double pbestPop[][dimlimit], double pbestFitness[], double gbestPop[], 
                      double *gbestFitness, int iter, int now_iter);    
void getFinalValue(const double pop[]);

#endif

