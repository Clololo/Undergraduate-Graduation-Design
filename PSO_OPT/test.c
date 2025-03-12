#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "FDE.h"
#include "pso.h"
#include "../tools/print_tool.h"

#define maxGen 5
#define sizePop 5
// const int dim = (vn_l + cn_l)/10 - 2;
int main() {
    srand(time(NULL));
    // 粒子群优化的参数
    double rangePop[2], rangeSpeed[2];
    getRangePop(rangePop);
    getRangeSpeed(rangeSpeed);

    //粒子群优化初始化
    double pop[sizePop][dimlimit], v[sizePop][dimlimit], fitness[sizePop];    
    printf("here");
    initPopVFit(sizePop, rangePop, rangeSpeed, pop, v, fitness, Ecn, Evn, 
            vn_degree, cn_degree, vn_edge_portion, cn_edge_portion);
    printf("here");
    // 初始最优解
    double gbestPop[sizePop], gbestFitness;    
    double pbestPop[sizePop][dimlimit], pbestFitness[sizePop];
    getInitBest(sizePop, fitness, pop, gbestPop, &gbestFitness, pbestPop, pbestFitness);
    printf("here");
    int lastBest = -1;
    int repeCounter = 0;

    // 粒子群优化迭代过程
    for (int gen = 0; gen < maxGen; ++gen) {
        printf("generation %d\n",gen+1);
        update_particles(sizePop, pop, v, fitness, pbestPop, pbestFitness, gbestPop, &gbestFitness, maxGen, gen);
        // printf(" %d: Best Fitness = %f\n", gen, gbestFitness);
    }
    
    // 输出优化后的结果
    // 输出优化后的 rho 和 lambda 值
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



