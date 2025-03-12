#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "FDE.h"
#include "pso.h"
#include "../tools/print_tool.h"

#define maxGen 50
#define sizePop 20
// const int dim = (vn_l + cn_l)/10 - 2;
int main() {
    srand(time(NULL));
    // ��ʼ���ȷֲ�  ��ʼ����ȫΪ2
    initialize_degree_distributions();
    // ����Ⱥ�Ż��Ĳ���
    double rangePop[2], rangeSpeed[2];
    getRangePop(rangePop);
    getRangeSpeed(rangeSpeed);

    //����Ⱥ�Ż���ʼ��
    double pop[sizePop][dimlimit], v[sizePop][dimlimit], fitness[sizePop];    

    initPopVFit(sizePop, rangePop, rangeSpeed, pop, v, fitness, Ecn, Evn, 
            vn_degree, cn_degree, vn_edge_portion, cn_edge_portion, 
          vn_l, cn_l);

    // ��ʼ���Ž�
    double gbestPop[sizePop], gbestFitness;    
    double pbestPop[sizePop][dimlimit], pbestFitness[sizePop];
    getInitBest(sizePop, fitness, pop, gbestPop, &gbestFitness, pbestPop, pbestFitness);

    int lastBest = -1;
    int repeCounter = 0;

    // ����Ⱥ�Ż���������
    for (int gen = 0; gen < maxGen; ++gen) {
        printf("generation %d\n",gen+1);
        update_particles(sizePop, pop, v, fitness, pbestPop, pbestFitness, gbestPop, &gbestFitness, maxGen, gen);
        // printf(" %d: Best Fitness = %f\n", gen, gbestFitness);
    }
    
    // ����Ż���Ľ��
    // ����Ż���� rho �� lambda ֵ
    printf("Optimized Degree Distributions:\n");
    printf("rho: ");
    for (int i = 1; i <= vn_l; i++) {
        printf("rho[%d] = %f ", i, rho[i]);
    }
    printf("\n");

    printf("lambda: ");
    for (int i = 1; i <= cn_l; i++) {
        printf("lambda[%d] = %f ", i, lambda[i]);
    }
    printf("\n");

    pso_save_to_csv("res_text/results.csv", rho, lambda, vn_l, cn_l);
    return 0;
}



