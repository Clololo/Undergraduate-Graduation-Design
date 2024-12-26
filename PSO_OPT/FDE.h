// 度分布函数 (ρ 和 λ)
#ifndef FDE_H
#define FDE_H

#define PI 3.141592653589793
#define MAX_ITER 1000     // 最大迭代次数
#define EPSILON 1e-6      // 收敛阖值
#define DV 6              // 最大变量节点度
#define DC 6              // 最大校验节点度
#define STEP_SIZE 0.1     // 拉普拉斯变换步长
#define SIGMA_TARGET 1.0  // 目标信噪比（可根据香农极限调整）
#define N 100             // 输入消息数量

// double rho[DV + 1];  // 变量节点的度分布概率
// double lambda[DC + 1];  // 校验节点的度分布概率
// double messages[N];  // 输入消息数组

void initialize_degree_distributions();
double compute_code_rate();
double variable_node_update(double sigma);
double check_node_update(double sigma);
double compute_shannon_limit(double code_rate);
double f_L(double L);
double laplace_transform(double *messages, double v);
double iterative_snr_threshold_FDE(double sigma_target);

#endif