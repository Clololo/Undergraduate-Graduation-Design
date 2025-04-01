#ifndef PSO_H
#define PSO_H

#include "../config.h"

extern int iters_tmp;


// ��ʼ����Ⱥ���ٶȺ���Ӧ��
void initPopVFit(int size, int codeLen, double pop[][codelength], double v[][codelength], double fitness[]);
// ��ȡ��ʼ����ֵ
void getInitBest(int size, int codeLen, double pop[][codelength], double v[][codelength], const double fitness[], double gbestPop[], double *gbestFitness,
    double pbestPop[][codelength], double pbestFitness[]);
//PSO����
double getDeclineRate(int iter,int now);
// ����Ⱥ�Ż����º�����������ʧ����������ũ���޵Ĳ�ֵ��ԽСԽ��
void update_particles(int size, int codeLen, double pop[][codelength], double v[][codelength], double fitness[],
    double pbestPop[][codelength], double pbestFitness[], double gbestPop[], double *gbestFitness);

#endif

