#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "FDE.h"
#include "pso.h"


// ��ʼ���ȷֲ���ʹ������΢�ֺ�Ϊ 1    
// ���������Ҫ����PSO�Ż����Ķȷֲ�    
// �Ӷ�2��ʼ����ΪLDPC����ƶ�Ϊ1�Ľڵ�


void initialize_degree_distributions() {    
    for (int i = 2; i <= vn_l; i++) {     
        rho[i] = 1.0 / (vn_l - 1);  // �Ӷ� 2 ��ʼ    
    }   
    for (int i = 2; i <= cn_l; i++) { 
        lambda[i] = 1.0 / (cn_l - 1);  // �Ӷ� 2 ��ʼ 
    }   
}   

// �������� R   
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

// �����ڵ����
double variable_node_update(double sigma) {
    double result = 0.0;
    for (int i = 2; i <= vn_l; i++) {
        result += rho[i] * tanh(sigma / 2.0);  // �����ڵ����Ϣ����
    }
    return result;
}

// У��ڵ����
double check_node_update(double sigma) {
    double result = 0.0;
    for (int i = 2; i <= cn_l; i++) {
        result += lambda[i] * exp(-sigma * i);  // У��ڵ����Ϣ����
    }
    return result;
}

// ������ũ����
double compute_shannon_limit(double code_rate) {
    return sqrt(2.0 * code_rate);  // ������ũ�ۣ����� �� = \sqrt{2R}
}

// ������Ϣ�ĸ����ܶȺ���������Ϊ��̬�ֲ���
double f_L(double L) {
    return exp(-L * L / 2.0) / sqrt(2 * M_PI);  // ��˹�ֲ�
}

// ������˹�任�����Ժ���
double laplace_transform(double *messages, double v) {
    double result = 0.0;
    for (int i = 0; i < N; i++) {
        result += pow(tanh(messages[i] / 2.0), v) * f_L(messages[i]);
    }
    return result * STEP_SIZE;
}

// ��������ֵ
double iterative_snr_threshold_FDE(double sigma_target) {
    double sigma = 0.5;  // ��ʼ
    int iter = 0;

    while (iter < MAX_ITER) {
        // �����ڵ�����
        double v_out = variable_node_update(sigma);

        // У��ڵ�����
        double c_out = check_node_update(sigma);

        // ������˹�任���Ժ���
        double laplace_result = laplace_transform(messages, 10.0);  // v = 10
        double sigma_new = v_out + c_out;

        // �ж���������
       if (fabs(sigma_new - sigma) < EPSILON) {
            printf("Converged at iteration %d, SNR threshold: %f, Laplace: %f\n", iter, sigma_new, laplace_result);
            return sigma_new;
        }

        sigma = sigma_new;  // ���������α�
        iter++;
    }

    printf("Did not converge within max iterations.\n");
    return sigma;
}

// int main() {
//     // ��ʼ���ȷֲ�
//     initialize_degree_distributions();

//     // �����ʼ��������Ϣ
//     for (int i = 0; i < N; i++) {
//         messages[i] = (rand() % 100) / 10.0 - 5.0;  // ������� -5 �� 5 ֮�����
//     }

//     // ��������
//     double code_rate = compute_code_rate();
//     printf("Code rate R: %f\n", code_rate);

//     // ������ũ����
//     double shannon_limit = compute_shannon_limit(code_rate);
//     printf("Shannon limit: %f\n", shannon_limit);

//     // ���������α���ֵ
//     double snr_threshold = iterative_snr_threshold(SIGMA_TARGET);
//     printf("Final SNR threshold: %f\n", snr_threshold);

//     // �Ƚ������α���ֵ����ũ���޵Ĳ�ֵ
//     double gap_to_shannon = snr_threshold - shannon_limit;
//     printf("Gap to Shannon limit: %f\n", gap_to_shannon);

//     return 0;
// }
