#ifndef PSO_H
#define PSO_H

#define EPSILON 1e-6      // 收敛阈值
#define STEP_SIZE 0.1     // 拉普拉斯变换步长
#define SIGMA_TARGET 0.8  // 初始模拟信噪比（可根据香农极限调整）  
#define PE_THRESHOLD 1e-5
#define SIGMA_INC 1e-3
#define GA_MODE 1
#define DE_MODE 2
#define MAX_ITER 800

//预期变量节点和校验节点的数量
#define pred_code_length 512
#define pred_vn_l 384      
#define pred_cn_l 128  

//预期的最低码率
#define pred_code_rate_lowlimit 0.75

//预期的最高码率
#define pre_code_rate_limit 0.9


#define vn_deg_max (((int)(sqrt(pred_cn_l)/2))+1)
#define cn_deg_max (((int)(sqrt(pred_vn_l)))+1)
#define dim (vn_deg_max+cn_deg_max)
#define dimlimit 128
#define alpha_penalty 10
#define beta_penalty 10

extern double Ecn[pred_cn_l], Evn[pred_vn_l];
extern double vn_degree[dimlimit], cn_degree[dimlimit];
extern double vn_edge_portion[dimlimit], cn_edge_portion[dimlimit];

extern double rho[dimlimit];  // 变量节点的度分布概率
extern double lambda[dimlimit];  // 校验节点的度分布概率
extern double messages[dimlimit];  // 输入消息数组

double getWeight();
void getLearningRate(double lr[2]);
void getRangePop(double rangePop[2]);
void getRangeSpeed(double rangeSpeed[2]);
double compute_code_rate(double rho[], double lambda[]);
double calculate_sigma_shannon(double R);
double func(const double x[], double *Ecn, double *Evn, 
    double *vn_degree, double *cn_degree, 
    double *vn_edge_portion, double *cn_edge_portion, double sum_exp_theta_r, double sum_exp_theta_l);
void initPopVFit(int sizePop, const double rangePop[2], const double rangeSpeed[2],
                 double pop[][dimlimit], double v[][dimlimit], double fitness[], double *Ecn, double *Evn, 
                double *vn_degree, double *cn_degree, 
                double *vn_edge_portion, double *cn_edge_portion);
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

