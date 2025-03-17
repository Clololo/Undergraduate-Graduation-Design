#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include "FDE.h"
#include "pso.h"
#include "GA.h"
#include <string.h>

double Ecn[pred_cn_l], Evn[pred_vn_l];
double vn_degree[dimlimit], cn_degree[dimlimit];
double vn_edge_portion[dimlimit], cn_edge_portion[dimlimit];
int pred_vn_length;
int pred_cn_length;
//当前的变量/校验节点的度的取值个数
int vn_deg_len = 0;
int cn_deg_len = 0;

// 校验节点的度分布概率
double rho[dimlimit];  

// 变量节点的度分布概率
double lambda[dimlimit];  

// 输入消息数组
double messages[dimlimit];  

//PSO直接优化的rho.lambda
//为了避免多余的归一化步骤
double theta_r[dimlimit];  
double theta_l[dimlimit];

// 获取惯性权重
double getWeight() {
    return 1.0;
}

// 获取学习因子
void getLearningRate(double lr[2]) {
    lr[0] = 0.39445;
    lr[1] = 0.89445;
}

// 粒子位置范围，以度数比
void getRangePop(double rangePop[2]) {
    rangePop[0] = 0.0;
    rangePop[1] = 1.0;
}

// 粒子速度范围
void getRangeSpeed(double rangeSpeed[2]) {
    rangeSpeed[0] = -0.05;
    rangeSpeed[1] = 0.05;
}

double iterative_snr_threshold
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
//考虑删除这个方法
void normalize_distribution(double *dist, int len) {   
    double sum = 0.0;
    for (int i = 0; i < len; i++) {
        sum += dist[i];
    }
    for (int i = 0; i < len; i++) {
        dist[i] /= sum;
    }
}

// //使用theta_r更新第pop_id个粒子的rho
// void update_rho(double sum_exp_theta, const double pop[]) {
//     for (int j = 2; j < cn_deg_max; j++) {
//         rho[j] = exp(pop[j]) / sum_exp_theta; // 更新 lambda[j] 的值
//     }
// }

// //使用theta_l更新第pop_id个粒子的lambda
// void update_lambda(double sum_exp_theta, const double pop[]) {
//     for (int j = cn_deg_max + 2; j < cn_deg_max + vn_deg_max; j++) {
//         lambda[j] = exp(pop[j]) / sum_exp_theta; // 更新 lambda[j] 的值
//     }
// }

void update_rho(double sum_exp_theta, const double pop[]) {
    for (int j = 2; j < cn_deg_max; j++) {
        rho[j] = pop[j] / sum_exp_theta; // 更新 lambda[j] 的值
    }
}

//使用theta_l更新第pop_id个粒子的lambda
void update_lambda(double sum_exp_theta, const double pop[]) {
    for (int j = cn_deg_max + 2; j < cn_deg_max + vn_deg_max; j++) {
        lambda[j - cn_deg_max] = pop[j] / sum_exp_theta; // 更新 lambda[j] 的值
    }
}

//由于lambda1可能为0的bug

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
        for (int i = 0; i < cn_deg_max; i++) {
            if (pos[i] > 0) {
                cn_degree[cn_cnt] = i;
                cn_edge_portion[cn_cnt] = pos[i];
                //printf("cn_degree[%d] = %d\n", cn_cnt, i);
                cn_cnt++;
            }
        }
        *cn_len = cn_cnt; // 存储 cn_degree 数组的长度
    } else if (mode == 2) {
    // 处理校验节点（CN），存入变量节点（VN）相关数组
        for (int i = 0; i < vn_deg_max; i++) {
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
// 该公式出自文章“The Optimized Scheme for FPGA-based  LDPC codes using Particle Swarm Optimization”
// 已修改
double compute_code_rate(double rho[], double lambda[]) {    

    double sum_rho = 0.0, sum_lambda = 0.0;
    for (int j = 1; j <= cn_deg_max; j++) {  
        sum_rho += rho[j] * (double)j;    
    }   
    for (int i = 1; i <= vn_deg_max; i++) {
        sum_lambda += lambda[i] * (double)i;
    }

    // 计算码率 R
    double R = 1.0 - (sum_lambda / sum_rho);
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
// 只需要提前给出pop,该函数内部完成pop2rho/lambda和后续计算
double func(const double x[], double *Ecn, double *Evn, 
            double *vn_degree, double *cn_degree, 
            double *vn_edge_portion, double *cn_edge_portion, double sum_exp_theta_r, double sum_exp_theta_l) {
    
    update_rho(sum_exp_theta_r, x);
    update_lambda(sum_exp_theta_l, x);

    double cn_degree_buf[dimlimit]; // 临时存储校验节点度分布
    double cn_edge_portion_buf[dimlimit]; // 临时存储校验节点边比例
    double vn_degree_buf[dimlimit]; // 临时存储变量节点度分布
    double vn_edge_portion_buf[dimlimit]; // 临时存储变量节点边比例

    pop2DegAndPortion(rho, cn_deg_max, 1, &cn_deg_len, &vn_deg_len, 
        cn_degree_buf, cn_edge_portion_buf,
        vn_degree_buf, vn_edge_portion_buf);

    pop2DegAndPortion(lambda, vn_deg_max, 2, &cn_deg_len, &vn_deg_len, 
        cn_degree_buf, cn_edge_portion_buf,
        vn_degree_buf, vn_edge_portion_buf);

    memcpy(cn_degree, cn_degree_buf, cn_deg_len * sizeof(double));
    memcpy(cn_edge_portion, cn_edge_portion_buf, cn_deg_len * sizeof(double));
    memcpy(vn_degree, vn_degree_buf, vn_deg_len * sizeof(double));
    memcpy(vn_edge_portion, vn_edge_portion_buf, vn_deg_len * sizeof(double));

    // for(int i = 0;i<cn_deg_len;i++){
    //     printf("cn_degree[%d] = %f\n",i,cn_degree[i]);
    // }
    // printf("vn_degree_len = %d\n",vn_deg_len);
    // for(int i = 0;i<vn_deg_len;i++){
    //     printf("vn_degree[%d] = %f\n",i,vn_degree[i]);
    // }
    // for(int i = 0;i<cn_deg_len;i++){
    //     printf("cn_degree_portion[%f] = %f\n",cn_degree[i],cn_edge_portion[i]);
    // }

    // for(int i = 0;i<vn_deg_len;i++){
    //     printf("vn_degree_portion[%f] = %f\n",vn_degree[i],vn_edge_portion[i]);
    // }
    double snr_threshold = iterative_snr_threshold(SIGMA_TARGET, 1, Ecn, Evn, 
                                                   vn_degree, cn_degree, 
                                                   vn_edge_portion, cn_edge_portion, 
                                                   vn_deg_len, cn_deg_len);
                        
    
  //  printf("snr_threshold = %f\n",snr_threshold);
    double shannon_limit = calculate_sigma_shannon(compute_code_rate(rho, lambda));   //码率计算近似的香农极限
  //  printf("shannon_limit = %f\n",shannon_limit);

    // 目标是最小化SNR和香农极限的差距,这个值要尽可能小！！
   // printf("sigma_star = %f\n",fabs(snr_threshold - shannon_limit));
    return fabs(snr_threshold - shannon_limit);
}

// 初始化种群、速度和适应度
void initPopVFit(int sizePop, const double rangePop[2], const double rangeSpeed[2],
                 double pop[][dimlimit], double v[][dimlimit], double fitness[], double *Ecn, double *Evn, 
                double *vn_degree, double *cn_degree, 
                double *vn_edge_portion, double *cn_edge_portion) {
    
   //处理第i个粒子
    //printf("init");
    for (int i = 0; i < sizePop; ++i) {
        double sum_exp_theta_r = 0.0;
        double sum_exp_theta_l = 0.0;
        for(int k = 0; k < dim; ++k) {
            if(k == 0 || k == 1 || k == cn_deg_max || k == cn_deg_max+1){
                pop[i][k] = 0;
                v[i][k] = 0;
            }else{
               // pop[i][k] = rand() / (double)RAND_MAX * rangePop[1];
                pop[i][k] = 0;
                v[i][k] = rand() / (double)RAND_MAX * rangeSpeed[1];
                // if(k >= 2 && k < cn_deg_max) sum_exp_theta_r += exp(pop[i][k]);
                // else if(k >= cn_deg_max + 2) sum_exp_theta_l += exp(pop[i][k]);
                // if(k >= 2 && k < cn_deg_max) sum_exp_theta_r += pop[i][k];
                // else if(k >= cn_deg_max + 2) sum_exp_theta_l += pop[i][k];
            }
        }
        pop[i][3] = 1.0;
        pop[i][cn_deg_max + 2] = 1.0;
        sum_exp_theta_r = 1.0;
        sum_exp_theta_l = 1.0;

        fitness[i] = func(pop[i], Ecn, Evn, vn_degree, cn_degree, \
                          vn_edge_portion, cn_edge_portion, sum_exp_theta_r, sum_exp_theta_l);

        //为了限制码率做出的惩罚
        double R = compute_code_rate(rho, lambda);
        fitness[i] += (alpha_penalty * pow(fmax(0, R - pre_code_rate_limit), 2) + beta_penalty * pow(fmax(0, pred_code_rate_lowlimit - R), 2));
    }
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
    double sum_exp_theta_r[sizePop];
    double sum_exp_theta_l[sizePop];
    //使用PSO公式更新速度
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
    
    //使用PSO公式更新位置
    for (int i = 0; i < sizePop; ++i) {
        sum_exp_theta_r[i] = 0.0;
        sum_exp_theta_l[i] = 0.0;
        for (int k = 0; k < dim; ++k) {
            pop[i][k] += v[i][k];   
            if (pop[i][k] < rangePop[0]) pop[i][k] = rangePop[0];
            if (pop[i][k] > rangePop[1]) pop[i][k] = rangePop[1];

            // if(k >= 2 && k < cn_deg_max) sum_exp_theta_r[i] += exp(pop[i][k]);
            // else if(k >= cn_deg_max + 2) sum_exp_theta_l[i] += exp(pop[i][k]);
            
            if(k >= 2 && k < cn_deg_max) sum_exp_theta_r[i] += pop[i][k];
            else if(k >= cn_deg_max + 2) sum_exp_theta_l[i] += pop[i][k];

        }
    }
    
    // 更新适应度和最优解
    for (int i = 0; i < sizePop; ++i) {

        //计算第i个粒子的损失函数
        fitness[i] = func(pop[i], Ecn, Evn, vn_degree, cn_degree, \
                          vn_edge_portion, cn_edge_portion, sum_exp_theta_r[i], sum_exp_theta_l[i]);
        
        //为了限制码率做出的惩罚
        double R = compute_code_rate(rho, lambda);
        fitness[i] += (alpha_penalty * pow(fmax(0, R - pre_code_rate_limit), 2) + beta_penalty * pow(fmax(0, pred_code_rate_lowlimit - R), 2));

        //更新粒子最优解
        if (fitness[i] < pbestFitness[i]) {
            pbestFitness[i] = fitness[i];
            for(int k = 0; k < dim; ++k){
                pbestPop[i][k] = pop[i][k];
            }
        }
        //更新全局最优解
        if (fitness[i] < *gbestFitness) {
            *gbestFitness = fitness[i];
            for(int k = 0; k < dim; ++k){
                gbestPop[k] = pop[i][k];
            }
        }
    }
}      

void getFinalValue(const double pop[]){
    double sum_exp_theta_r = 0.0;
    double sum_exp_theta_l = 0.0;
    for(int i = 0; i < dim; i++){
        if(i >= 2 && i < cn_deg_max) sum_exp_theta_r += pop[i];
        else if(i >= cn_deg_max + 2) sum_exp_theta_l += pop[i];
    }
    update_rho(sum_exp_theta_r,pop);
    update_lambda(sum_exp_theta_l,pop);
}