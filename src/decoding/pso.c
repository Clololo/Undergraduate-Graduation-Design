#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include "pso.h"
#include "../config.h"
#include <string.h>

// 获取惯性权重
double getWeight() {
    return 1.0;
}

// 获取学习因子
void getLearningRate(double lr[2]) {
    lr[0] = 5.49445;
    lr[1] = 10.19445;
}

// 粒子位置范围，以度数比
void getRangePop(double rangePop[2]) {
    rangePop[0] = 0.0;
    rangePop[1] = 1.0;
}

// 粒子速度范围
void getRangeSpeed(double rangeSpeed[2]) {
    rangeSpeed[0] = -1;
    rangeSpeed[1] = 1;
}


// 初始化种群、速度和适应度
void initPopVFit(int size, int codeLen, double pop[][codelength], double v[][codelength], double fitness[]) {
    double rangeSpeed[2];
    getRangeSpeed(rangeSpeed);
    //处理第i个粒子
    for (int i = 0; i < sizePop_de; ++i) {
        for(int k = 0; k < codeLen; ++k) {
            pop[i][k] = 1;   
            v[i][k] = rand() / (double)RAND_MAX * rangeSpeed[1];  
        }
        fitness[i] = init_fitness;
    }
}


// 获取初始最优值
void getInitBest(int size, int codeLen, double pop[][codelength], double v[][codelength], const double fitness[], double gbestPop[], double *gbestFitness,
    double pbestPop[][codelength], double pbestFitness[]) {
    int maxIdx = 0;                 
    for (int i = 0; i < sizePop_de; ++i) {
        if (fitness[i] > fitness[maxIdx]) {
            maxIdx = i;
        }
    }   
    //看是哪一个粒子搜索到了最佳值
    *gbestFitness = fitness[maxIdx];
    for(int i = 0; i < sizePop_de; ++i) {
        gbestPop[i] =  pop[maxIdx][i];
    }
    for (int i = 0; i < sizePop_de; ++i) {
        pbestFitness[i] = fitness[i];
        for(int k = 0; k < codeLen; ++k){
            pbestPop[i][k] = pop[i][k];
        }
    }
}

//PSO参数
double getDeclineRate(int iter,int now){
    return ((double)iter-(double)now)/(double)iter;
}

// 粒子群优化更新函数，这里损失函数（与香农极限的差值）越小越好
void update_particles(int size, int codeLen, double pop[][codelength], double v[][codelength], double fitness[],
    double pbestPop[][codelength], double pbestFitness[], double gbestPop[], double *gbestFitness) {
    // pop[][dim]  代表若干粒子，每个粒子搜索dim个自变量的当前解
    // v[][dim]  若干粒子在每个自变量上的速度
    // fitness 每个粒子的最优适应度
    // pbestPop  每个粒子搜索dim个自变量的最优解
    // gbestPop  dim个自变量的全局最优解
    double lr[2];
    getLearningRate(lr);
    double rangeSpeed[2] = {-0.15, 0.15};   // 速度的上下限
    double rangePop[2] = {0, 1};   //位置的上下限
    for (int i = 0; i < sizePop_de; ++i) {
        for (int k = 0; k < codeLen; ++k) {
            // double w = getDeclineRate(iter,now_iter);
            double w = 1.0;
            v[i][k] += w * (lr[0] * ((double)rand() / RAND_MAX) * (pbestPop[i][k] - pop[i][k])
                    + lr[1] * ((double)rand() / RAND_MAX) * (gbestPop[k] - pop[i][k]));
            if (v[i][k] < rangeSpeed[0]) v[i][k] = rangeSpeed[0];
            if (v[i][k] > rangeSpeed[1]) v[i][k] = rangeSpeed[1];
        }
    }
    //使用PSO公式更新位置
    for (int i = 0; i < sizePop_de; ++i) {
        for (int k = 0; k < codeLen; ++k) {
            pop[i][k] += v[i][k];   
            if (pop[i][k] < rangePop[0]) pop[i][k] = rangePop[0];   // 下限
        }
    }
    // 更新适应度和最优解
    for (int i = 0; i < sizePop_de; ++i) {
        //计算第i个粒子的损失函数
        //这里的fitness[i]的更新写在了update_particles的外部
        //更新粒子最优解
        if (fitness[i] > pbestFitness[i]) {
            pbestFitness[i] = fitness[i];
            for(int k = 0; k < codeLen; ++k){
                pbestPop[i][k] = pop[i][k];
            }
        }
        //更新全局最优解
        if (fitness[i] > *gbestFitness) {
            *gbestFitness = fitness[i];
            for(int k = 0; k < codeLen; ++k){
                gbestPop[k] = pop[i][k];
            }
        }
    }
}   