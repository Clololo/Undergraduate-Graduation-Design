#ifndef PSO_H
#define PSO_H

#define PI 3.141592653589793
#define EPSILON 1e-6      // ������ֵ
#define STEP_SIZE 0.1     // ������˹�任����
#define SIGMA_TARGET 1.0  // Ŀ������ȣ��ɸ�����ũ���޵�����
#define MAX_ITER 1000
#define PE_THRESHOLD 1e-5
#define SIGMA_INC 1e-3
#define GA_MODE 1
#define DE_MODE 2

#define all_len 150
#define vn_l 100
#define cn_l 50


extern int vn_len;
extern int cn_len;
extern const int dim;

extern double Ecn[cn_l], Evn[vn_l];
extern double vn_degree[vn_l], cn_degree[cn_l];
extern double vn_edge_portion[vn_l], cn_edge_portion[cn_l];

extern double rho[vn_l + 1];  // �����ڵ�Ķȷֲ�����
extern double lambda[cn_l + 1];  // У��ڵ�Ķȷֲ�����
extern double messages[cn_l];  // ������Ϣ����

double getWeight();
void getLearningRate(double lr[2]);
int getMaxGen();
int getSizePop();
void getRangePop(double rangePop[2]);
void getRangeSpeed(double rangeSpeed[2]);
double func(const double x[], double *Ecn, double *Evn, 
            double *vn_degree, double *cn_degree, 
            double *vn_edge_portion, double *cn_edge_portion, 
            int vn_len, int cn_len);
void initPopVFit(int sizePop, const double rangePop[2], const double rangeSpeed[2],
                 double pop[][dim], double v[][dim], double fitness[], double *Ecn, double *Evn, 
                double *vn_degree, double *cn_degree, 
                double *vn_edge_portion, double *cn_edge_portion, 
                int vn_len, int cn_len);
void getInitBest(int sizePop, const double fitness[], const double pop[][dim],
                 double gbestPop[], double *gbestFitness,
                 double pbestPop[], double pbestFitness[]);
int iterative_snr_threshold(double SIGMA, int key, double *Ecn, 
                            double *Evn, double *vn_degree, double *cn_degree, 
                            double *vn_edge_portion, double *cn_edge_portion, 
                            int vn_len, int cn_len);
void update_particles(int sizePop, double pop[][dim], double v[][dim], double fitness[],
                      double pbestPop[][dim], double pbestFitness[], double gbestPop[], 
                      double *gbestFitness, int iter, int now_iter);    

#endif
