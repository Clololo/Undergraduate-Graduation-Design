#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include "FDE.h"
#include "pso.h"
#include "GA.h"


//const int dim = 150;

double Ecn[cn_l], Evn[vn_l];
double vn_degree[vn_l], cn_degree[cn_l];
double vn_edge_portion[vn_l], cn_edge_portion[cn_l];

double rho[vn_l + 1];  // 变量节点的度分布概率
double lambda[cn_l + 1];  // 校验节点的度分布概率
double messages[dim];  // 输入消息数组

// 获取惯性权重
double getWeight() {
    return 1.0;
}

// 获取学习因子
void getLearningRate(double lr[2]) {
    lr[0] = 0.49445;
    lr[1] = 1.49445;
}

// 最大迭代次数
int getMaxGen() {
    return 200;
}

// 种群规模
int getSizePop() {
    return all_len;
}

// 粒子位置范围
void getRangePop(double rangePop[2]) {
    rangePop[0] = 0;
    rangePop[1] = 1;
}

// 粒子速度范围
void getRangeSpeed(double rangeSpeed[2]) {
    rangeSpeed[0] = -0.2;
    rangeSpeed[1] = 0.2;
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

//归一化函数
void normalize_distribution(double *dist, int len) {   
    double sum = 0.0;
    for (int i = 0; i < len; i++) {
        sum += dist[i];
    }
    for (int i = 0; i < len; i++) {
        dist[i] /= sum;
    }
}

// pop2DegAndPortion函数
// 把rho和lambda中的格式换成func需要的一个度对应一个比例的格式
void pop2DegAndPortion(double *pos, int len, int mode) {
    int cn_cnt = 0;
    int vn_cnt = 0;
    if(mode == 1){  
        //vn
        for (int i = 0; i < len; i++) {
            if(pos[i] > 0)
            {
                cn_degree[cn_cnt] = i;
                cn_edge_portion[cn_cnt] = pos[i];
                cn_cnt ++;
            }
        }
    }else if(mode == 2){
        //cn
        for (int i = 0; i < len; i++) {
            if(pos[i] > 0)
            {
                vn_degree[vn_cnt] = i;
                vn_edge_portion[vn_cnt] = pos[i];
                vn_cnt ++;
            }
        }
    }else{
        printf("wrong mode!\n");
    }
}

// 损失函数  
double func(const double x[], double *Ecn, double *Evn, 
            double *vn_degree, double *cn_degree, 
            double *vn_edge_portion, double *cn_edge_portion, 
            int vn_len, int cn_len) {

    for (int i = 0; i < cn_len; i++) {
        rho[i] = x[i];  // 从粒子数组中提取校验节点度分布
    }

    for (int i = 0; i < vn_len; i++) {
        lambda[i] = x[cn_len + i];  // 从粒子数组中提取变量节点度分布
    }

    normalize_distribution(rho, cn_len);
    normalize_distribution(lambda, vn_len);

    pop2DegAndPortion(rho, cn_len, 1);
    pop2DegAndPortion(lambda, vn_len, 2);
    
    // 使用信道解码中的逻辑来计算SNR
    double snr_threshold = iterative_snr_threshold(SIGMA_TARGET, 1, Ecn, Evn, 
                                                   vn_degree, cn_degree, 
                                                   vn_edge_portion, cn_edge_portion, 
                                                   vn_len, cn_len);
   // printf("snr_threshold = %f\n",snr_threshold);
    double shannon_limit = compute_shannon_limit(compute_code_rate());
    //printf("shannon_limit = %f\n",shannon_limit);
    // 目标是最小化SNR和香农极限的差距
    return fabs(snr_threshold - shannon_limit);
}

// 初始化种群、速度和适应度
void initPopVFit(int sizePop, const double rangePop[2], const double rangeSpeed[2],
                 double pop[][dim], double v[][dim], double fitness[], double *Ecn, double *Evn, 
                double *vn_degree, double *cn_degree, 
                double *vn_edge_portion, double *cn_edge_portion, 
                int vn_len, int cn_len) {
    for (int i = 0; i < sizePop; ++i) {
        for(int k = 0; k < dim; ++k) {
            pop[i][k] = rand() / (double)RAND_MAX * rangePop[1];
            v[i][k] = rand() / (double)RAND_MAX * rangeSpeed[1];
        }
        fitness[i] = func(pop[i], Ecn, Evn, vn_degree, cn_degree, \
                          vn_edge_portion, cn_edge_portion, vn_len, cn_len);
    }
}

// 获取初始最优值
void getInitBest(int sizePop, const double fitness[], const double pop[][dim],
                 double gbestPop[], double *gbestFitness,
                 double pbestPop[][dim], double pbestFitness[]) {
    int maxIdx = 0;                 
    for (int i = 1; i < sizePop; ++i) {
        if (fitness[i] > fitness[maxIdx]) {
            maxIdx = i;
        }
    }   //看是哪一个粒子搜索到了最佳值
    
    *gbestFitness = fitness[maxIdx];
    for(int i = 0; i < sizePop; ++i){
        gbestPop[i] =  pop[maxIdx][i];
    }

    for (int i = 0; i < sizePop; ++i) {
        pbestFitness[i] = fitness[i];
        for(int k = 0; k < dim; ++k){
            pbestPop[i][k] = pop[i][k];
        }
    }
}

double getDeclineRate(int iter,int now){
    return ((double)iter-(double)now)/(double)iter;
}

// 粒子群优化更新函数
void update_particles(int sizePop, double pop[][dim], double v[][dim], double fitness[],
                      double pbestPop[][dim], double pbestFitness[], double gbestPop[], 
                      double *gbestFitness, int iter, int now_iter) {

    // pop[][dim]  代表若干粒子，每个粒子搜索dim个自变量的当前解
    // v[][dim]  若干粒子在每个自变量上的速度
    // fitness 每个粒子的最优适应度
    // pbestPop  每个粒子搜索dim个自变量的最优解
    // gbestPop  dim个自变量的全局最优解
    double lr[2];
    getLearningRate(lr);
    double rangeSpeed[2] = {-0.2, 0.2};   // 速度的上下限
    double rangePop[2] = {0, 1};   //位置的上下限

    for (int i = 0; i < sizePop; ++i) {
        for (int k = 0; k < dim; ++k) {
            double w = getDeclineRate(iter,now_iter);
            v[i][k] += w * (lr[0] * ((double)rand() / RAND_MAX) * (pbestPop[i][k] - pop[i][k])
                    + lr[1] * ((double)rand() / RAND_MAX) * (gbestPop[k] - pop[i][k]));
            if (v[i][k] < rangeSpeed[0]) v[i][k] = rangeSpeed[0];
            if (v[i][k] > rangeSpeed[1]) v[i][k] = rangeSpeed[1];
        }
    }

    for (int i = 0; i < sizePop; ++i) {
        for (int k = 0; k < dim; ++k) {
            pop[i][k] += v[i][k];
            if (pop[i][k] < rangePop[0]) pop[i][k] = rangePop[0];
            if (pop[i][k] > rangePop[1]) pop[i][k] = rangePop[1];
        }
    }

    // 更新适应度和最优解
    for (int i = 0; i < sizePop; ++i) {
        fitness[i] = func(pop[i], Ecn, Evn, vn_degree, cn_degree, \
                          vn_edge_portion, cn_edge_portion, vn_l, cn_l);
        //printf("now fitness i = %f\n", fitness[i]);
        if (fitness[i] > pbestFitness[i]) {
            pbestFitness[i] = fitness[i];
            for(int k = 0; k < dim; ++k){
                pbestPop[i][k] = pop[i][k];
            }
        }
        if (fitness[i] > *gbestFitness) {
            *gbestFitness = fitness[i];
            for(int k = 0; k < dim; ++k){
                gbestPop[k] = pop[i][k];
            }
        }
    }
}      
