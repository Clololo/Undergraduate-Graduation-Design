#ifndef PSO_H
#define PSO_H


#define GA_MODE 1
#define DE_MODE 2

#define vn_l 100
#define cn_l 50

extern int vn_len;
extern int cn_len;

extern double Ecn[cn_l], Evn[vn_l];
extern double vn_degree[vn_l], cn_degree[cn_l];
extern double vn_edge_portion[vn_l], cn_edge_portion[cn_l];

extern double rho[DV + 1];  // 变量节点的度分布概率
extern double lambda[DC + 1];  // 校验节点的度分布概率
extern double messages[N];  // 输入消息数组

double getWeight();
void getLearningRate(double lr[2]);
int getMaxGen();
int getSizePop();
void getRangePop(double rangePop[2]);
void getRangeSpeed(double rangeSpeed[2]);
double func(const double x[2], double *Ecn, double *Evn, 
            double *vn_degree, double *cn_degree, 
            double *vn_edge_portion, double *cn_edge_portion, 
            int vn_len, int cn_len);
void initPopVFit(int sizePop, const double rangePop[2], const double rangeSpeed[2],
                 double pop[][2], double v[][2], double fitness[], double *Ecn, double *Evn, 
                double *vn_degree, double *cn_degree, 
                double *vn_edge_portion, double *cn_edge_portion, 
                int vn_len, int cn_len);
void getInitBest(int sizePop, const double fitness[], const double pop[][2],
                 double gbestPop[2], double *gbestFitness,
                 double pbestPop[][2], double pbestFitness[]);
int iterative_snr_threshold(double SIGMA, int key, double *Ecn, 
                            double *Evn, double *vn_degree, double *cn_degree, 
                            double *vn_edge_portion, double *cn_edge_portion, 
                            int vn_len, int cn_len);
void update_particles(int sizePop, double pop[][2], double v[][2], double fitness[],
                      double pbestPop[][2], double pbestFitness[], double gbestPop[2], 
                      double *gbestFitness);
                             
#endif
