// �ȷֲ����� (�� �� ��)
#ifndef FDE_H
#define FDE_H

#define PI 3.141592653589793
#define MAX_ITER 1000     // ����������
#define EPSILON 1e-6      // ������ֵ
#define DV 6              // �������ڵ��
#define DC 6              // ���У��ڵ��
#define STEP_SIZE 0.1     // ������˹�任����
#define SIGMA_TARGET 1.0  // Ŀ������ȣ��ɸ�����ũ���޵�����
#define N 100             // ������Ϣ����

// double rho[DV + 1];  // �����ڵ�Ķȷֲ�����
// double lambda[DC + 1];  // У��ڵ�Ķȷֲ�����
// double messages[N];  // ������Ϣ����

void initialize_degree_distributions();
double compute_code_rate();
double variable_node_update(double sigma);
double check_node_update(double sigma);
double compute_shannon_limit(double code_rate);
double f_L(double L);
double laplace_transform(double *messages, double v);
double iterative_snr_threshold_FDE(double sigma_target);

#endif