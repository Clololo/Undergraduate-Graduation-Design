#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "FDE.h"
#include "pso.h"
#include "../tools/print_tool.h"
#include "../tools/math_tool.h"

#define maxGen 200
#define sizePop 30

// const int dim = (pred_vn_l + pred_cn_l)/10 - 2;
int main() {
    //clock_t start_time = clock();
    srand(time(NULL));
    // 粒子群优化的参数
    double rangePop[2], rangeSpeed[2];
    getRangePop(rangePop);
    getRangeSpeed(rangeSpeed);
    
    //粒子群优化初始化
    double pop[sizePop][dimlimit], v[sizePop][dimlimit], fitness[sizePop];    
    initPopVFit(sizePop, rangePop, rangeSpeed, pop, v, fitness, Ecn, Evn, 
            vn_degree, cn_degree, vn_edge_portion, cn_edge_portion);

    // 初始最优解
    double gbestPop[sizePop], gbestFitness;    
    double pbestPop[sizePop][dimlimit], pbestFitness[sizePop];
    getInitBest(sizePop, fitness, pop, gbestPop, &gbestFitness, pbestPop, pbestFitness);
    int lastBest = -1;
    int repeCounter = 0;

    // 粒子群优化迭代过程
    for (int gen = 0; gen < maxGen; ++gen) {
        //printf("gen%d\n",gen+1);
        if((gen*10) % maxGen == 0){
            printf("finished %.0f%% \n",(double)(100*(double)gen/(double)maxGen));
        }
        update_particles(sizePop, pop, v, fitness, pbestPop, pbestFitness, gbestPop, &gbestFitness, maxGen, gen);
    }
    
    // 输出优化后的结果
    // 输出优化后的 rho 和 lambda 值
    printf("best dif = %f\n",gbestFitness);
    getFinalValue(gbestPop);
    // printf("Optimized Degree Distributions:\n");
    // printf("rho: \n");
    // for (int i = 2; i <= cn_deg_max; i++) {
    //     printf("rho[%d] = %f\n", i, rho[i]);
    // }
    // printf("\n------------------------------------------------\n");

    // printf("lambda:\n ");
    // for (int i = 2; i <= vn_deg_max; i++) {
    //     printf("lambda[%d] = %f\n", i, lambda[i]);
    // }
    // printf("\n");
    
    double R = compute_code_rate(rho, lambda);
    printf("Searched best code rate = %f\n", R);
    
    pso_save_to_csv("temp/opt_deg.csv", rho, lambda, cn_deg_max, vn_deg_max);
    pso_save_to_csv("output/opt_deg.csv", rho, lambda, cn_deg_max, vn_deg_max);

    pred_vn_length = pred_code_length;
    pred_cn_length = approx((double)(pred_vn_length)*(1.0 - R));

    write_number_to_csv("temp/pred_vnl.csv", pred_vn_length);
    write_number_to_csv("temp/pred_cnl.csv", pred_cn_length);
    int edge1 = pegsrc_save_to_csv("temp/sim_rho.csv", rho, lambda, pred_cn_length , cn_deg_max, vn_deg_max,  1);
    int edge2 = pegsrc_save_to_csv("temp/sim_lambda.csv", rho, lambda, pred_vn_length, cn_deg_max, vn_deg_max, 2);
    
    int edge_diff = edge2 - edge1;
    printf("%d edges diff\n",edge_diff);

    //clock_t end_time = clock();
    // 计算运行时间
    //double elapsed_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;
    // 打印运行时间
    //printf("total running time: %.1f ms\n", elapsed_time*1000);

    return 0;
}



