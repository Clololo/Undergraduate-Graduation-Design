#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include "FDE.h"
#include "pso.h"
#include "GA.h"
#include <string.h>

double Ecn[cn_l], Evn[vn_l];
double vn_degree[dimlimit], cn_degree[dimlimit];
double vn_edge_portion[dimlimit], cn_edge_portion[dimlimit];

double rho[dimlimit];  // 变量节点的度分布概率
double lambda[dimlimit];  // 校验节点的度分布概率
double messages[dimlimit];  // 输入消息数组

// 获取惯性权重
double getWeight() {
    return 1.0;
}

// 获取学习因子
void getLearningRate(double lr[2]) {
    lr[0] = 0.49445;
    lr[1] = 1.49445;
}

// 粒子位置范围，以度数比
void getRangePop(double rangePop[2]) {
    rangePop[0] = 0.0;
    rangePop[1] = 1.0;
}

// 粒子速度范围
void getRangeSpeed(double rangeSpeed[2]) {
    rangeSpeed[0] = -0.2;
    rangeSpeed[1] = 0.2;
}

//初始化使初始度数全为2.
void initialize_degree_distributions() {    
    for (int i = 2; i <= vn_l; i++) {     
        rho[i] = (i == 2) ? 1.0 : 0.0;  // 只有度为2的变量节点有概率
    }   
    for (int i = 3; i <= cn_l; i++) { 
        lambda[i] = (i == 2) ? 1.0 : 0.0;  // 只有度为3的校验节点有概率
    }   
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
    int vn_deg_len, 
    int cn_deg_len
){
    if(key == GA_MODE){
        return iterative_snr_threshold_GA(SIGMA, Ecn, Evn,\
                                        vn_degree, cn_degree,\
                                        vn_edge_portion, cn_edge_portion,\
                                        vn_deg_len, cn_deg_len);
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
void pop2DegAndPortion(double *pos, int len, int mode, 
    int *cn_len, int *vn_len,
    double *cn_degree, double *cn_edge_portion,
    double *vn_degree, double *vn_edge_portion) {
    int cn_cnt = 0;
    int vn_cnt = 0;

    if (mode == 1) { 
    // 处理变量节点（VN），存入校验节点（CN）相关数组
    for (int i = 0; i < len; i++) {
        if (pos[i] > 0) {
            cn_degree[cn_cnt] = i;
            printf("cn_degree[%d] = %d\n", cn_cnt, i);
            cn_edge_portion[cn_cnt] = pos[i];
            cn_cnt++;
        }
    }
    *cn_len = cn_cnt; // 存储 cn_degree 数组的长度
    } else if (mode == 2) {
    // 处理校验节点（CN），存入变量节点（VN）相关数组
        for (int i = 0; i < len; i++) {
            if (pos[i] > 0) {
                vn_degree[vn_cnt] = i;
                vn_edge_portion[vn_cnt] = pos[i];
                vn_cnt++;
            }
        }
        *vn_len = vn_cnt; // 存储 vn_degree 数组的长度
    } 
    else {
        printf("wrong mode!\n");
    }
}

// 根据度分布，计算码率 R ，在func中调用
double compute_code_rate() {    
    double sum_rho = 0.0, sum_lambda = 0.0; 
    double rhosum = 0.0;
    double lamsum = 0.0;
    for (int i = 2; i <= vn_l; i++) {  
        sum_rho += i * rho[i];    
    }   
    for (int i = 2; i <= cn_l; i++) {
        sum_lambda += i * lambda[i];
    }
    double R = 1.0 - (sum_rho / sum_lambda);
    return R;
}

// 计算特定码率对应的噪声标准差，在func中调用
double calculate_sigma_shannon(double R) {
    // 检查码率是否在有效范围内
    if (R <= 0 || R >= 1) {
        printf("Invalid rate. R must be between 0 and 1.\n");
        return -1;  // 返回一个错误标识
    }
    
    // 根据公式计算 σ*
    double sigma_shannon = 1.0 / sqrt(2 * (pow(2, R) - 1));
    return sigma_shannon;
}

// 损失函数，计算当前度分布下的噪声阈值与香农极限下理论噪声阈值的差
double func(const double x[], double *Ecn, double *Evn, 
            double *vn_degree, double *cn_degree, 
            double *vn_edge_portion, double *cn_edge_portion, 
            int vn_len, int cn_len) {
    
    // 从粒子数组中提取校验节点度分布
    for (int i = 0; i < cn_deg_max; i++) {
        rho[i] = x[i];  
    }

    // 从粒子数组中提取变量节点度分布
    for (int i = cn_deg_max,j = 0; i < cn_deg_max+vn_deg_max; i++,j++) {
        lambda[j] = x[i];  
    }

    normalize_distribution(rho, cn_len);
    normalize_distribution(lambda, vn_len);


    int vn_deg_len = 0;
    int cn_deg_len = 0;

    double cn_degree_buf[dimlimit]; // 临时存储校验节点度分布
    double cn_edge_portion_buf[dimlimit]; // 临时存储校验节点边比例
    double vn_degree_buf[dimlimit]; // 临时存储变量节点度分布
    double vn_edge_portion_buf[dimlimit]; // 临时存储变量节点边比例

    pop2DegAndPortion(rho, cn_len, 1, &cn_deg_len, &vn_deg_len, 
        cn_degree_buf, cn_edge_portion_buf,
        vn_degree_buf, vn_edge_portion_buf);

    pop2DegAndPortion(lambda, vn_len, 2, &cn_deg_len, &vn_deg_len, 
        cn_degree_buf, cn_edge_portion_buf,
        vn_degree_buf, vn_edge_portion_buf);

    memcpy(cn_degree, cn_degree_buf, cn_deg_len * sizeof(double));
    memcpy(cn_edge_portion, cn_edge_portion_buf, cn_deg_len * sizeof(double));
    memcpy(vn_degree, vn_degree_buf, vn_deg_len * sizeof(double));
    memcpy(vn_edge_portion, vn_edge_portion_buf, vn_deg_len * sizeof(double));

    // for(int i = 0;i<cn_deg_len;i++){
    //     printf("cn_degree[%d] = %d\n",i,cn_degree[i]);
    // }
    // for(int i = 0;i<vn_deg_len;i++){
    //     printf("vn_degree[%d] = %d\n",i,vn_degree[i]);
    // }
    // for(int i = 0;i<cn_deg_len;i++){
    //     printf("cn_degree_portion[%f] = %f\n",cn_degree[i],cn_edge_portion[i]);
    // }

    // for(int i = 0;i<vn_deg_len;i++){
    //     printf("vn_degree_portion[%f] = %f\n",vn_degree[i],vn_edge_portion[i]);
    // }
    // 使用信道解码中的逻辑来计算SNR
    // int vn_deg_len = cn_l / 10;
    // int cn_deg_len = vn_l / 10;

    double snr_threshold = iterative_snr_threshold(SIGMA_TARGET, 1, Ecn, Evn, 
                                                   vn_degree, cn_degree, 
                                                   vn_edge_portion, cn_edge_portion, 
                                                   vn_deg_len, cn_deg_len);
    
    printf("snr_threshold = %f\n",snr_threshold);
    double shannon_limit = calculate_sigma_shannon(compute_code_rate());   //码率计算近似的香农极限
    printf("shannon_limit = %f\n",shannon_limit);

    // 目标是最小化SNR和香农极限的差距,这个值要尽可能小！！
    return fabs(snr_threshold - shannon_limit);
}

// 初始化种群、速度和适应度
void initPopVFit(int sizePop, const double rangePop[2], const double rangeSpeed[2],
                 double pop[][dimlimit], double v[][dimlimit], double fitness[], double *Ecn, double *Evn, 
                double *vn_degree, double *cn_degree, 
                double *vn_edge_portion, double *cn_edge_portion, 
                int vn_len, int cn_len) {
    printf("init begin\n");
    for (int i = 0; i < sizePop; ++i) {
      //  printf("pop%d\n",i);
        for(int k = 0; k < dim; ++k) {
            if(k == 0 || k == 1 || k == cn_deg_max || k == cn_deg_max+1){
                pop[i][k] = 0;
                v[i][k] = 0;
            }else{
                pop[i][k] = rand() / (double)RAND_MAX * rangePop[1];
                v[i][k] = rand() / (double)RAND_MAX * rangeSpeed[1];
            }
        }
        //printf("pop%d\n",i);
        fitness[i] = func(pop[i], Ecn, Evn, vn_degree, cn_degree, \
                          vn_edge_portion, cn_edge_portion, vn_len, cn_len);
    }
    printf("init end\n");
}

// 获取初始最优值
void getInitBest(int sizePop, const double fitness[], const double pop[][dimlimit],
                 double gbestPop[], double *gbestFitness,
                 double pbestPop[][dimlimit], double pbestFitness[]) {
    int maxIdx = 0;                 
    for (int i = 0; i < sizePop; ++i) {
        if (fitness[i] < fitness[maxIdx]) {
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

//PSO参数
double getDeclineRate(int iter,int now){
    return ((double)iter-(double)now)/(double)iter;
}

// 粒子群优化更新函数，这里损失函数（与香农极限的差值）越小越好
void update_particles(int sizePop, double pop[][dimlimit], double v[][dimlimit], double fitness[],
                      double pbestPop[][dimlimit], double pbestFitness[], double gbestPop[], 
                      double *gbestFitness, int iter, int now_iter) {

    // pop[][dim]  代表若干粒子，每个粒子搜索dim个自变量的当前解
    // v[][dim]  若干粒子在每个自变量上的速度
    // fitness 每个粒子的最优适应度
    // pbestPop  每个粒子搜索dim个自变量的最优解
    // gbestPop  dim个自变量的全局最优解
    double lr[2];
    getLearningRate(lr);
    double rangeSpeed[2] = {-0.1, 0.1};   // 速度的上下限
    double rangePop[2] = {0, 1};   //位置的上下限

    //更新速度
    for (int i = 0; i < sizePop; ++i) {
        for (int k = 0; k < dim; ++k) {
            if(k == 0 || k == 1 || k == cn_deg_max || k == cn_deg_max+1) v[i][k] = 0; //不考虑度数为0，1的  
            else{
                double w = getDeclineRate(iter,now_iter);
                v[i][k] += w * (lr[0] * ((double)rand() / RAND_MAX) * (pbestPop[i][k] - pop[i][k])
                        + lr[1] * ((double)rand() / RAND_MAX) * (gbestPop[k] - pop[i][k]));
                if (v[i][k] < rangeSpeed[0]) v[i][k] = rangeSpeed[0];
                if (v[i][k] > rangeSpeed[1]) v[i][k] = rangeSpeed[1];
            }
        }
    }

    //更新位置
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
        if (fitness[i] < pbestFitness[i]) {
            pbestFitness[i] = fitness[i];
            for(int k = 0; k < dim; ++k){
                pbestPop[i][k] = pop[i][k];
            }
        }
        if (fitness[i] < *gbestFitness) {
            *gbestFitness = fitness[i];
            for(int k = 0; k < dim; ++k){
                gbestPop[k] = pop[i][k];
            }
        }
    }
}      