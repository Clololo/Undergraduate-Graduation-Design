#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include "FDE.h"
#include "pso.h"
#include "GA.h"


int vn_len = 100;  // �����ڵ���
int cn_len = 50;   // У��ڵ���

const int dim = 150;

double Ecn[cn_l], Evn[vn_l];
double vn_degree[vn_l], cn_degree[cn_l];
double vn_edge_portion[vn_l], cn_edge_portion[cn_l];

double rho[vn_len + 1];  // �����ڵ�Ķȷֲ�����
double lambda[cn_len + 1];  // У��ڵ�Ķȷֲ�����
double messages[dim];  // ������Ϣ����

// ��ȡ����Ȩ��
double getWeight() {
    return 1.0;
}

// ��ȡѧϰ����
void getLearningRate(double lr[2]) {
    lr[0] = 0.49445;
    lr[1] = 1.49445;
}

// ����������
int getMaxGen() {
    return 200;
}

// ��Ⱥ��ģ
int getSizePop() {
    return all_len;
}

// ����λ�÷�Χ
void getRangePop(double rangePop[2]) {
    rangePop[0] = -2 * PI;
    rangePop[1] = 2 * PI;
}

// �����ٶȷ�Χ
void getRangeSpeed(double rangeSpeed[2]) {
    rangeSpeed[0] = -0.5;
    rangeSpeed[1] = 0.5;
}


int iterative_snr_threshold
    (double SIGMA, 
    int key, 
    double *Ecn, 
    double *Evn, 
    double *vn_degree, 
    double *cn_degree, 
    double *vn_edge_portion, 
    double *cn_edge_portion, 
    int vn_len, 
    int cn_len
){
    if(key == GA_MODE){
        return iterative_snr_threshold_GA(SIGMA, Ecn, Evn,\
                                        vn_degree, cn_degree,\
                                        vn_edge_portion, cn_edge_portion,\
                                        vn_len, cn_len);
    }
    else if(key == DE_MODE){
        return iterative_snr_threshold_FDE(SIGMA);
    }
    else{
        printf("wrong in mode chosen\n");
        return -1;
    }
}

//��ʧ����
void normalize_distribution(double *dist, int len) {   
    double sum = 0.0;
    for (int i = 0; i < len; i++) {
        sum += dist[i];
    }
    for (int i = 0; i < len; i++) {
        dist[i] /= sum;
    }
}

// ��ʧ����  
double func(const double x[], double *Ecn, double *Evn, 
            double *vn_degree, double *cn_degree, 
            double *vn_edge_portion, double *cn_edge_portion, 
            int vn_len, int cn_len) {

    for (int i = 0; i < cn_len; i++) {
        rho[i] = x[i];  // ��������������ȡУ��ڵ�ȷֲ�
    }

    for (int i = 0; i < vn_len; i++) {
        lambda[i] = x[cn_len + i];  // ��������������ȡ�����ڵ�ȷֲ�
    }

    normalize_distribution(rho, cn_len);
    normalize_distribution(lambda, vn_len);
    
    // ʹ���ŵ������е��߼�������SNR
    double snr_threshold = iterative_snr_threshold(SIGMA_TARGET, 1, Ecn, Evn, 
                                                   vn_degree, cn_degree, 
                                                   vn_edge_portion, cn_edge_portion, 
                                                   vn_len, cn_len);
    double shannon_limit = compute_shannon_limit(compute_code_rate());
    
    // Ŀ������С��SNR����ũ���޵Ĳ��
    return fabs(snr_threshold - shannon_limit);
}

// ��ʼ����Ⱥ���ٶȺ���Ӧ��
void initPopVFit(int sizePop, const double rangePop[2], const double rangeSpeed[2],
                 double pop[][dim], double v[][dim], double fitness[], double *Ecn, double *Evn, 
                double *vn_degree, double *cn_degree, 
                double *vn_edge_portion, double *cn_edge_portion, 
                int vn_len, int cn_len) {
    for (int i = 0; i < sizePop; ++i) {
        for(int k = 0; k < dim; ++k) {
            pop[i][k] = (rand() / (double)RAND_MAX - 0.5) * rangePop[1] * 2;
            v[i][k] = (rand() / (double)RAND_MAX - 0.5) * rangeSpeed[1] * 2;
        }
        fitness[i] = func(pop[i], Ecn, Evn, vn_degree, cn_degree, \
                          vn_edge_portion, cn_edge_portion, vn_len, cn_len);
    }
}

// ��ȡ��ʼ����ֵ
void getInitBest(int sizePop, const double fitness[], const double pop[][dim],
                 double gbestPop[], double *gbestFitness,
                 double pbestPop[], double pbestFitness[]) {
    int maxIdx = 0;                 
    for (int i = 1; i < sizePop; ++i) {
        if (fitness[i] > fitness[maxIdx]) {
            maxIdx = i;
        }
    }   //������һ�����������������ֵ
    
    *gbestFitness = fitness[maxIdx];
    gbestPop = pop[maxIdx];

    for (int i = 0; i < sizePop; ++i) {
        pbestFitness[i] = fitness[i];
        for(int k = 0; k < dim; ++k){
            pbestPop[i][k] = pop[i][k];
        }
    }
}

double getDeclineRate(int iter,int now){
    return ((double)iter-(double)now)/(double)iter;
}

// ����Ⱥ�Ż����º���
void update_particles(int sizePop, double pop[][dim], double v[][dim], double fitness[],
                      double pbestPop[][dim], double pbestFitness[], double gbestPop[], 
                      double *gbestFitness, int iter, int now_iter) {

    // pop[][dim]  �����������ӣ�ÿ����������dim���Ա����ĵ�ǰ��
    // v[][dim]  ����������ÿ���Ա����ϵ��ٶ�
    // fitness ÿ�����ӵ�������Ӧ��
    // pbestPop  ÿ����������dim���Ա��������Ž�
    // gbestPop  dim���Ա�����ȫ�����Ž�
    double lr[2];
    getLearningRate(lr);
    double rangeSpeed[2] = {-0.5, 0.5};   // �ٶȵ�������
    double rangePop[2] = {0, 1};   //λ�õ�������

    for (int i = 0; i < sizePop; ++i) {
        for (int k = 0; k < dim; ++k) {
            double w = getDeclineRate(iter,now_iter);
            v[i][k] += w * (lr[0] * ((double)rand() / RAND_MAX) * (pbestPop[i][k] - pop[i][k])
                    + lr[1] * ((double)rand() / RAND_MAX) * (gbestPop[k] - pop[i][k]));
            if (v[i][k] < rangeSpeed[0]) v[i][k] = rangeSpeed[0];
            if (v[i][k] > rangeSpeed[1]) v[i][k] = rangeSpeed[1];
        }
    }

    for (int i = 0; i < sizePop; ++i) {
        for (int k = 0; k < dim; ++k) {
            pop[i][k] += v[i][k];
            if (pop[i][k] < rangePop[0]) pop[i][k] = rangePop[0];
            if (pop[i][k] > rangePop[1]) pop[i][k] = rangePop[1];
        }
    }

    // ������Ӧ�Ⱥ����Ž�
    for (int i = 0; i < sizePop; ++i) {
        fitness[i] = func(pop[i], Ecn, Evn, vn_degree, cn_degree, \
                          vn_edge_portion, cn_edge_portion, vn_len, cn_len);
        if (fitness[i] > pbestFitness[i]) {
            pbestFitness[i] = fitness[i];
            for(int k = 0; k < dim; ++k){
                pbestPop[i][k] = pop[i][k];
            }
        }
        if (fitness[i] > *gbestFitness) {
            *gbestFitness = fitness[i];
            for(int k = 0; k < dim; ++k){
                gbestPop[k] = pop[i][k];
            }
        }
    }
}      
