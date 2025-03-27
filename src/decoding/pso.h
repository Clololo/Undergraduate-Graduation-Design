#ifndef PSO_H
#define PSO_H

#include "../config.h"

extern int iters_tmp;

// 获取惯性权重
double getWeight();
// 获取学习因子
void getLearningRate(double lr[2]);
// 粒子位置范围，以度数比
void getRangePop(double rangePop[2]);
// 粒子速度范围
void getRangeSpeed(double rangeSpeed[2]);
//损失函数
// 初始化种群、速度和适应度
void initPopVFit(int size, int codeLen, double pop[][codelength], double v[][codelength], double fitness[]);
// 获取初始最优值
void getInitBest(int size, int codeLen, double pop[][codelength], double v[][codelength], const double fitness[], double gbestPop[], double *gbestFitness,
    double pbestPop[][codelength], double pbestFitness[]);
//PSO参数
double getDeclineRate(int iter,int now);
// 粒子群优化更新函数，这里损失函数（与香农极限的差值）越小越好
void update_particles(int size, int codeLen, double pop[][codelength], double v[][codelength], double fitness[],
    double pbestPop[][codelength], double pbestFitness[], double gbestPop[], double *gbestFitness);

#endif

