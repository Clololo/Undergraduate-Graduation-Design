// 度分布函数 (ρ 和 λ)
#ifndef FDE_H
#define FDE_H



// double rho[DV + 1];  // 变量节点的度分布概率
// double lambda[DC + 1];  // 校验节点的度分布概率
// double messages[N];  // 输入消息数组

double compute_code_rate();
double variable_node_update(double sigma);
double check_node_update(double sigma);
double compute_shannon_limit(double code_rate);
double f_L(double L);
double laplace_transform(double *messages, double v);
double iterative_snr_threshold_FDE(double sigma_target);

#endif