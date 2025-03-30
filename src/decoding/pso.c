#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include "pso.h"
#include "../config.h"
#include <string.h>



// ��ʼ����Ⱥ���ٶȺ���Ӧ��
void initPopVFit(int size, int codeLen, double pop[][codelength], double v[][codelength], double fitness[]) {
    //�����i������
    for (int i = 0; i < sizePop_de; ++i) {
        for(int k = 0; k < codeLen; ++k) {
            pop[i][k] = 1.0;   
            v[i][k] = rand() / (double)RAND_MAX * de_rangepophigh;  
        }
        fitness[i] = init_fitness;
    }
}


// ��ȡ��ʼ����ֵ
void getInitBest(int size, int codeLen, double pop[][codelength], double v[][codelength], const double fitness[], double gbestPop[], double *gbestFitness,
    double pbestPop[][codelength], double pbestFitness[]) {
    int maxIdx = 0;                 
    for (int i = 0; i < sizePop_de; ++i) {
        if (fitness[i] > fitness[maxIdx]) {
            maxIdx = i;
        }
    }   
    //������һ�����������������ֵ
    *gbestFitness = fitness[maxIdx];
    for(int i = 0; i < codeLen; ++i) {
        gbestPop[i] =  pop[maxIdx][i];
    }
    for (int i = 0; i < sizePop_de; ++i) {
        pbestFitness[i] = fitness[i];
        for(int k = 0; k < codeLen; ++k){
            pbestPop[i][k] = pop[i][k];
        }
    }
}

//PSO����
double getDeclineRate(int iter,int now){
    return ((double)iter-(double)now)/(double)iter;
}

// ����Ⱥ�Ż����º�����������ʧ����������ũ���޵Ĳ�ֵ��ԽСԽ��
void update_particles(int size, int codeLen, double pop[][codelength], double v[][codelength], double fitness[],
    double pbestPop[][codelength], double pbestFitness[], double gbestPop[], double *gbestFitness) {
    // pop[][dim]  �����������ӣ�ÿ����������dim���Ա����ĵ�ǰ��
    // v[][dim]  ����������ÿ���Ա����ϵ��ٶ�
    // fitness ÿ�����ӵ�������Ӧ��
    // pbestPop  ÿ����������dim���Ա��������Ž�
    // gbestPop  dim���Ա�����ȫ�����Ž�

    for (int i = 0; i < sizePop_de; ++i) {
        for (int k = 0; k < codeLen; ++k) {
            // double w = getDeclineRate(iter,now_iter);
            double w = 1.0;
            v[i][k] += w * (de_lr0 * ((double)rand() / RAND_MAX) * (pbestPop[i][k] - pop[i][k])
                    + de_lr1 * ((double)rand() / RAND_MAX) * (gbestPop[k] - pop[i][k]));
            if (v[i][k] < de_rangevlow) v[i][k] = de_rangevlow;
            if (v[i][k] > de_rangevhigh) v[i][k] = de_rangevhigh;
        }
    }
    //ʹ��PSO��ʽ����λ��
    for (int i = 0; i < sizePop_de; ++i) {
        for (int k = 0; k < codeLen; ++k) {
            pop[i][k] += v[i][k];   
            if (pop[i][k] < de_rangepoplow) pop[i][k] = de_rangepoplow;   // ����
        }
    }
    // ������Ӧ�Ⱥ����Ž�
    for (int i = 0; i < sizePop_de; ++i) {
        //�����i�����ӵ���ʧ����
        //�����fitness[i]�ĸ���д����update_particles���ⲿ
        //�����������Ž�
        if (fitness[i] > pbestFitness[i]) {
            pbestFitness[i] = fitness[i];
            for(int k = 0; k < codeLen; ++k){
                pbestPop[i][k] = pop[i][k];
            }
        }
        //����ȫ�����Ž�
        if (fitness[i] > *gbestFitness) {
            *gbestFitness = fitness[i];
            for(int k = 0; k < codeLen; ++k){
                gbestPop[k] = pop[i][k];
            }
        }
    }
}   