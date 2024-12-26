#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include "FDE.h"
#include "pso.h"
#include "GA.h"

#define PI 3.141592653589793


int vn_len = 100;  // �����ڵ���
int cn_len = 50;   // У��ڵ���


double Ecn[cn_l], Evn[vn_l];
double vn_degree[vn_l], cn_degree[cn_l];
double vn_edge_portion[vn_l], cn_edge_portion[cn_l];

double rho[DV + 1];  // �����ڵ�Ķȷֲ�����
double lambda[DC + 1];  // У��ڵ�Ķȷֲ�����
double messages[N];  // ������Ϣ����

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
    return 50;
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

//��JB

// ��ʧ����  
double func(const double x[2], double *Ecn, double *Evn, 
            double *vn_degree, double *cn_degree, 
            double *vn_edge_portion, double *cn_edge_portion, 
            int vn_len, int cn_len) {
    // x[0] -> rho[2] ��ֵ, x[1] -> lambda[2] ��ֵ
    rho[2] = x[0];
    lambda[2] = x[1];
    
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
                 double pop[][2], double v[][2], double fitness[], double *Ecn, double *Evn, 
                double *vn_degree, double *cn_degree, 
                double *vn_edge_portion, double *cn_edge_portion, 
                int vn_len, int cn_len) {
    for (int i = 0; i < sizePop; ++i) {
        pop[i][0] = (rand() / (double)RAND_MAX - 0.5) * rangePop[1] * 2;
        pop[i][1] = (rand() / (double)RAND_MAX - 0.5) * rangePop[1] * 2;
        v[i][0] = (rand() / (double)RAND_MAX - 0.5) * rangeSpeed[1] * 2;
        v[i][1] = (rand() / (double)RAND_MAX - 0.5) * rangeSpeed[1] * 2;
        fitness[i] = func(pop[i], Ecn, Evn, vn_degree, cn_degree, \
                          vn_edge_portion, cn_edge_portion, vn_len, cn_len);
    }
}

// ��ȡ��ʼ����ֵ
void getInitBest(int sizePop, const double fitness[], const double pop[][2],
                 double gbestPop[2], double *gbestFitness,
                 double pbestPop[][2], double pbestFitness[]) {
    int maxIdx = 0;
    for (int i = 1; i < sizePop; ++i) {
        if (fitness[i] > fitness[maxIdx]) {
            maxIdx = i;
        }
    }
    *gbestFitness = fitness[maxIdx];
    gbestPop[0] = pop[maxIdx][0];
    gbestPop[1] = pop[maxIdx][1];

    for (int i = 0; i < sizePop; ++i) {
        pbestFitness[i] = fitness[i];
        pbestPop[i][0] = pop[i][0];
        pbestPop[i][1] = pop[i][1];
    }
}

// ����Ⱥ�Ż����º���
void update_particles(int sizePop, double pop[][2], double v[][2], double fitness[],
                      double pbestPop[][2], double pbestFitness[], double gbestPop[2], 
                      double *gbestFitness) {
    double lr[2];
    getLearningRate(lr);
    double rangeSpeed[2] = {-0.5, 0.5};
    double rangePop[2] = {-2 * PI, 2 * PI};

    for (int i = 0; i < sizePop; ++i) {
        for (int k = 0; k < 2; ++k) {
            v[i][k] += lr[0] * ((double)rand() / RAND_MAX) * (pbestPop[i][k] - pop[i][k])
                    + lr[1] * ((double)rand() / RAND_MAX) * (gbestPop[k] - pop[i][k]);
            if (v[i][k] < rangeSpeed[0]) v[i][k] = rangeSpeed[0];
            if (v[i][k] > rangeSpeed[1]) v[i][k] = rangeSpeed[1];
        }
    }

    for (int i = 0; i < sizePop; ++i) {
        for (int k = 0; k < 2; ++k) {
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
            pbestPop[i][0] = pop[i][0];
            pbestPop[i][1] = pop[i][1];
        }
        if (fitness[i] > *gbestFitness) {
            *gbestFitness = fitness[i];
            gbestPop[0] = pop[i][0];
            gbestPop[1] = pop[i][1];
        }
    }
}

// int main() {
//     srand(time(0));

//     double w = getWeight();
//     double lr[2];
//     getLearningRate(lr);
//     int maxGen = getMaxGen();
//     int sizePop = getSizePop();

//     double rangePop[2];
//     getRangePop(rangePop);
//     double rangeSpeed[2];
//     getRangeSpeed(rangeSpeed);

//     double pop[sizePop][2];
//     double v[sizePop][2];
//     double fitness[sizePop];
//     initPopVFit(sizePop, rangePop, rangeSpeed, pop, v, fitness);

//     double gbestPop[2];
//     double gbestFitness;
//     double pbestPop[sizePop][2];
//     double pbestFitness[sizePop];
//     getInitBest(sizePop, fitness, pop, gbestPop, &gbestFitness, pbestPop, pbestFitness);

//     double result[maxGen];
//     for (int i = 0; i < maxGen; ++i) {
//         double t = 0.5;

//         // �����ٶ�
//         for (int j = 0; j < sizePop; ++j) {
//             for (int k = 0; k < 2; ++k) {
//                 v[j][k] += lr[0] * ((double)rand() / RAND_MAX) * (pbestPop[j][k] - pop[j][k])
//                         + lr[1] * ((double)rand() / RAND_MAX) * (gbestPop[k] - pop[j][k]);
//                 if (v[j][k] < rangeSpeed[0]) v[j][k] = rangeSpeed[0];
//                 if (v[j][k] > rangeSpeed[1]) v[j][k] = rangeSpeed[1];
//             }
//         }

//         // ����λ��
//         for (int j = 0; j < sizePop; ++j) {
//             for (int k = 0; k < 2; ++k) {
//                 pop[j][k] = t * (0.5 * v[j][k]) + (1 - t) * pop[j][k];
//                 if (pop[j][k] < rangePop[0]) pop[j][k] = rangePop[0];
//                 if (pop[j][k] > rangePop[1]) pop[j][k] = rangePop[1];
//             }
//         }

//         // ������Ӧ��
//         for (int j = 0; j < sizePop; ++j) {
//             fitness[i] = func(pop[i], Ecn, Evn, vn_degree, cn_degree, \
//                           vn_edge_portion, cn_edge_portion, vn_len, cn_len);

//             if (fitness[j] > pbestFitness[j]) {
//                 pbestFitness[j] = fitness[j];
//                 pbestPop[j][0] = pop[j][0];
//                 pbestPop[j][1] = pop[j][1];
//             }
//         }

//         int maxIdx = 0;
//         for (int j = 1; j < sizePop; ++j) {
//             if (pbestFitness[j] > pbestFitness[maxIdx]) {
//                 maxIdx = j;
//             }
//         }
//         if (pbestFitness[maxIdx] > gbestFitness) {
//             gbestFitness = pbestFitness[maxIdx];
//             gbestPop[0] = pbestPop[maxIdx][0];
//             gbestPop[1] = pbestPop[maxIdx][1];
//         }

//         result[i] = gbestFitness;
//         printf("��%d��ѭ������[%f, %f]�õ������Ӧ��%f\n",i+1,gbestPop[0], gbestPop[1],gbestFitness);
//     }

//     // ������ս��
//     printf("�����Ӧ��: %f\n", gbestFitness);
//     printf("���λ��: [%f, %f]\n", gbestPop[0], gbestPop[1]);

//     return 0;
// }
