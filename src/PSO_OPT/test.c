#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "FDE.h"
#include "pso.h"
#include <time.h>
#include "../tools/print_tool.h"

#define maxGen 1000
#define sizePop 100
// const int dim = (pred_vn_l + pred_cn_l)/10 - 2;
int main() {

    clock_t start_time = clock();
    srand(time(NULL));
    // ����Ⱥ�Ż��Ĳ���
    double rangePop[2], rangeSpeed[2];
    getRangePop(rangePop);
    getRangeSpeed(rangeSpeed);
    
    //����Ⱥ�Ż���ʼ��
    double pop[sizePop][dimlimit], v[sizePop][dimlimit], fitness[sizePop];    
    initPopVFit(sizePop, rangePop, rangeSpeed, pop, v, fitness, Ecn, Evn, 
            vn_degree, cn_degree, vn_edge_portion, cn_edge_portion);

    // ��ʼ���Ž�
    double gbestPop[sizePop], gbestFitness;    
    double pbestPop[sizePop][dimlimit], pbestFitness[sizePop];
    getInitBest(sizePop, fitness, pop, gbestPop, &gbestFitness, pbestPop, pbestFitness);
    int lastBest = -1;
    int repeCounter = 0;

    // ����Ⱥ�Ż���������
    for (int gen = 0; gen < maxGen; ++gen) {
        if(gen % 100 == 0) printf("finished %.0f%% \n",(double)(100*(double)gen/(double)maxGen));
        update_particles(sizePop, pop, v, fitness, pbestPop, pbestFitness, gbestPop, &gbestFitness, maxGen, gen);
        // printf(" %d: Best Fitness = %f\n", gen, gbestFitness);
    }
    
    // ����Ż���Ľ��
    // ����Ż���� rho �� lambda ֵ
    printf("best dif = %f\n",gbestFitness);
    // printf("")

    getFinalValue(gbestPop);
    printf("Optimized Degree Distributions:\n");
    printf("rho: \n");
    for (int i = 2; i <= cn_deg_max; i++) {
        printf("rho[%d] = %f\n", i, rho[i]);
    }
    printf("\n------------------------------------------------\n");

    printf("lambda:\n ");
    for (int i = 2; i <= vn_deg_max; i++) {
        printf("lambda[%d] = %f\n", i, lambda[i]);
    }
    printf("\n");

    printf("the best R = %f\n",compute_code_rate());
    
    pso_save_to_csv("../out/results.csv", rho, lambda, cn_deg_max, vn_deg_max);

    clock_t end_time = clock();
    // ��������ʱ��
    double elapsed_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;
    // ��ӡ����ʱ��
    printf("total running time: %.1f ms\n", elapsed_time*1000);

    return 0;
}



