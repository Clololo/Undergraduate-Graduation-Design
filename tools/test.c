#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "FDE.h"
#include "pso.h"

// #define PI 3.141592653589793
// #define MAX_ITER 1000     // 最大迭代次数
// #define EPSILON 1e-6      // 收敛值
// #define DV 6              // 最大变量节点度
// #define DC 6              // 最大校验节点度
// #define STEP_SIZE 0.1     // 拉普拉斯变换步长
// #define SIGMA_TARGET 1.0  // 目标信器治比 σ（可根据香农极限调整）
// #define N 100             // 输入消息数量

// // 度分布函数 (ρ 和 λ)
// double rho[DV + 1];  // 变量节点的度分布概率
// double lambda[DC + 1];  // 校验节点的度分布概率
// double messages[N];  // 输入消息数组


int main() {
    srand(time(NULL));

    // 初始化度分布
    initialize_degree_distributions();

    // 粒子群优化的参数
    int sizePop = getSizePop();
    int maxGen = getMaxGen();
    double rangePop[2], rangeSpeed[2];
    getRangePop(rangePop);
    getRangeSpeed(rangeSpeed);

    // 粒子群优化初始化
    double pop[sizePop][2], v[sizePop][2], fitness[sizePop];
    initPopVFit(sizePop, rangePop, rangeSpeed, pop, v, fitness, Ecn, Evn, 
            vn_degree, cn_degree, vn_edge_portion, cn_edge_portion, 
            vn_len, cn_len);
    
    // 初始最优解
    double gbestPop[2], gbestFitness;
    double pbestPop[sizePop][2], pbestFitness[sizePop];
    getInitBest(sizePop, fitness, pop, gbestPop, &gbestFitness, pbestPop, pbestFitness);

    int lastBest = -1;
    int repeCounter = 0;
    // 粒子群优化迭代过程
    for (int gen = 0; gen < maxGen; ++gen) {
        update_particles(sizePop, pop, v, fitness, pbestPop, pbestFitness, gbestPop, &gbestFitness);
        printf(" %d: Best Fitness = %f\n", gen, gbestFitness);
        // if(lastBest == gbestFitness){
        //     repeCounter++;
        //     if(repeCounter >= 100) break;
        // }
        // else{
        //     repeCounter = 0;
        // }
        // lastBest = gbestFitness;
    }

    // 输出优化后的结果
    printf("Optimized rho[2]: %f, lambda[2]: %f\n", gbestPop[0], gbestPop[1]);

    return 0;
}


