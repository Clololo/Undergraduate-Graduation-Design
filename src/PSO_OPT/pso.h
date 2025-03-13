#ifndef PSO_H
#define PSO_H

#define EPSILON 1e-6      // ������ֵ
#define STEP_SIZE 0.1     // ������˹�任����
#define SIGMA_TARGET 0.8  // ��ʼģ������ȣ��ɸ�����ũ���޵�����  
#define PE_THRESHOLD 1e-5
#define SIGMA_INC 1e-3
#define GA_MODE 1
#define DE_MODE 2
#define MAX_ITER 500

//�����ڵ��У��ڵ������
#define all_len 192 
#define vn_l 128      
#define cn_l 64   


#define vn_deg_max (((int)(sqrt(cn_l)/2))+1)
#define cn_deg_max (((int)(sqrt(vn_l)))+1)
#define dim (vn_deg_max+cn_deg_max)
#define dimlimit 128
#define alpha_penalty 1
#define beta_penalty 1
// extern int vn_l;
// extern int cn_l;

extern double Ecn[cn_l], Evn[vn_l];
extern double vn_degree[dimlimit], cn_degree[dimlimit];
extern double vn_edge_portion[dimlimit], cn_edge_portion[dimlimit];

extern double rho[dimlimit];  // �����ڵ�Ķȷֲ�����
extern double lambda[dimlimit];  // У��ڵ�Ķȷֲ�����
extern double messages[dimlimit];  // ������Ϣ����

double getWeight();
void getLearningRate(double lr[2]);
void getRangePop(double rangePop[2]);
void getRangeSpeed(double rangeSpeed[2]);
double compute_code_rate();
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

