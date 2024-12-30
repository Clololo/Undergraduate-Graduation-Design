#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "FDE.h"
#include "pso.h"


// 初始化度分布，使其满足微分和为 1    
// 这里后面需要换成PSO优化出的度分布    
// 从度2开始是因为LDPC不设计度为1的节点


void initialize_degree_distributions() {    
    for (int i = 2; i <= vn_l; i++) {     
        rho[i] = 1.0 / (vn_l - 1);  // 从度 2 开始    
    }   
    for (int i = 2; i <= cn_l; i++) { 
        lambda[i] = 1.0 / (cn_l - 1);  // 从度 2 开始 
    }   
}   

// 计算码率 R   
double compute_code_rate() {    
    double sum_rho = 0.0, sum_lambda = 0.0; 
    for (int i = 2; i <= vn_l; i++) {  
        sum_rho += rho[i];    
    }   
    for (int i = 2; i <= cn_l; i++) {
        sum_lambda += lambda[i];
    }
    return 1.0 - (sum_rho / sum_lambda);
}

// 变量节点更新
double variable_node_update(double sigma) {
    double result = 0.0;
    for (int i = 2; i <= vn_l; i++) {
        result += rho[i] * tanh(sigma / 2.0);  // 变量节点的消息更新
    }
    return result;
}

// 校验节点更新
double check_node_update(double sigma) {
    double result = 0.0;
    for (int i = 2; i <= cn_l; i++) {
        result += lambda[i] * exp(-sigma * i);  // 校验节点的消息更新
    }
    return result;
}

// 计算香农极限
double compute_shannon_limit(double code_rate) {
    return sqrt(2.0 * code_rate);  // 根据香农论，极限 σ = \sqrt{2R}
}

// 计算消息的概率密度函数（假设为正态分布）
double f_L(double L) {
    return exp(-L * L / 2.0) / sqrt(2 * M_PI);  // 高斯分布
}

// 拉普拉斯变换的特性函数
double laplace_transform(double *messages, double v) {
    double result = 0.0;
    for (int i = 0; i < N; i++) {
        result += pow(tanh(messages[i] / 2.0), v) * f_L(messages[i]);
    }
    return result * STEP_SIZE;
}

// 计算门限值
double iterative_snr_threshold_FDE(double sigma_target) {
    double sigma = 0.5;  // 初始
    int iter = 0;

    while (iter < MAX_ITER) {
        // 变量节点的输出
        double v_out = variable_node_update(sigma);

        // 校验节点的输出
        double c_out = check_node_update(sigma);

        // 拉普拉斯变换特性函数
        double laplace_result = laplace_transform(messages, 10.0);  // v = 10
        double sigma_new = v_out + c_out;

        // 判断收敛条件
       if (fabs(sigma_new - sigma) < EPSILON) {
            printf("Converged at iteration %d, SNR threshold: %f, Laplace: %f\n", iter, sigma_new, laplace_result);
            return sigma_new;
        }

        sigma = sigma_new;  // 更新信器治比
        iter++;
    }

    printf("Did not converge within max iterations.\n");
    return sigma;
}

// int main() {
//     // 初始化度分布
//     initialize_degree_distributions();

//     // 随机初始化输入消息
//     for (int i = 0; i < N; i++) {
//         messages[i] = (rand() % 100) / 10.0 - 5.0;  // 随机生成 -5 到 5 之间的数
//     }

//     // 计算码率
//     double code_rate = compute_code_rate();
//     printf("Code rate R: %f\n", code_rate);

//     // 计算香农极限
//     double shannon_limit = compute_shannon_limit(code_rate);
//     printf("Shannon limit: %f\n", shannon_limit);

//     // 计算信器治比阖值
//     double snr_threshold = iterative_snr_threshold(SIGMA_TARGET);
//     printf("Final SNR threshold: %f\n", snr_threshold);

//     // 比较信器治比阖值与香农极限的差值
//     double gap_to_shannon = snr_threshold - shannon_limit;
//     printf("Gap to Shannon limit: %f\n", gap_to_shannon);

//     return 0;
// }
