// �ȷֲ����� (�� �� ��)
#ifndef FDE_H
#define FDE_H



// double rho[DV + 1];  // �����ڵ�Ķȷֲ�����
// double lambda[DC + 1];  // У��ڵ�Ķȷֲ�����
// double messages[N];  // ������Ϣ����

double compute_code_rate();
double variable_node_update(double sigma);
double check_node_update(double sigma);
double compute_shannon_limit(double code_rate);
double f_L(double L);
double laplace_transform(double *messages, double v);
double iterative_snr_threshold_FDE(double sigma_target);

#endif