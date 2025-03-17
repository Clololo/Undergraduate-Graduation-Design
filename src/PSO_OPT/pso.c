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
//��ǰ�ı���/У��ڵ�Ķȵ�ȡֵ����
int vn_deg_len = 0;
int cn_deg_len = 0;

// У��ڵ�Ķȷֲ�����
double rho[dimlimit];  

// �����ڵ�Ķȷֲ�����
double lambda[dimlimit];  

// ������Ϣ����
double messages[dimlimit];  

//PSOֱ���Ż���rho.lambda
//Ϊ�˱������Ĺ�һ������
double theta_r[dimlimit];  
double theta_l[dimlimit];

// ��ȡ����Ȩ��
double getWeight() {
    return 1.0;
}

// ��ȡѧϰ����
void getLearningRate(double lr[2]) {
    lr[0] = 0.39445;
    lr[1] = 0.89445;
}

// ����λ�÷�Χ���Զ�����
void getRangePop(double rangePop[2]) {
    rangePop[0] = 0.0;
    rangePop[1] = 1.0;
}

// �����ٶȷ�Χ
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
 
//��һ������ 
//����ɾ���������
void normalize_distribution(double *dist, int len) {   
    double sum = 0.0;
    for (int i = 0; i < len; i++) {
        sum += dist[i];
    }
    for (int i = 0; i < len; i++) {
        dist[i] /= sum;
    }
}

// //ʹ��theta_r���µ�pop_id�����ӵ�rho
// void update_rho(double sum_exp_theta, const double pop[]) {
//     for (int j = 2; j < cn_deg_max; j++) {
//         rho[j] = exp(pop[j]) / sum_exp_theta; // ���� lambda[j] ��ֵ
//     }
// }

// //ʹ��theta_l���µ�pop_id�����ӵ�lambda
// void update_lambda(double sum_exp_theta, const double pop[]) {
//     for (int j = cn_deg_max + 2; j < cn_deg_max + vn_deg_max; j++) {
//         lambda[j] = exp(pop[j]) / sum_exp_theta; // ���� lambda[j] ��ֵ
//     }
// }

void update_rho(double sum_exp_theta, const double pop[]) {
    for (int j = 2; j < cn_deg_max; j++) {
        rho[j] = pop[j] / sum_exp_theta; // ���� lambda[j] ��ֵ
    }
}

//ʹ��theta_l���µ�pop_id�����ӵ�lambda
void update_lambda(double sum_exp_theta, const double pop[]) {
    for (int j = cn_deg_max + 2; j < cn_deg_max + vn_deg_max; j++) {
        lambda[j - cn_deg_max] = pop[j] / sum_exp_theta; // ���� lambda[j] ��ֵ
    }
}

//����lambda1����Ϊ0��bug

// pop2DegAndPortion����
// ��rho��lambda�еĸ�ʽ����func��Ҫ��һ���ȶ�Ӧһ�������ĸ�ʽ
void pop2DegAndPortion(double *pos, int len, int mode, 
    int *cn_len, int *vn_len,
    double *cn_degree, double *cn_edge_portion,
    double *vn_degree, double *vn_edge_portion) {
    int cn_cnt = 0;
    int vn_cnt = 0;


    if (mode == 1) { 
        // ��������ڵ㣨VN��������У��ڵ㣨CN���������
        for (int i = 0; i < cn_deg_max; i++) {
            if (pos[i] > 0) {
                cn_degree[cn_cnt] = i;
                cn_edge_portion[cn_cnt] = pos[i];
                //printf("cn_degree[%d] = %d\n", cn_cnt, i);
                cn_cnt++;
            }
        }
        *cn_len = cn_cnt; // �洢 cn_degree ����ĳ���
    } else if (mode == 2) {
    // ����У��ڵ㣨CN������������ڵ㣨VN���������
        for (int i = 0; i < vn_deg_max; i++) {
            if (pos[i] > 0) {
                vn_degree[vn_cnt] = i;
                vn_edge_portion[vn_cnt] = pos[i];
                vn_cnt++;
            }
        }
        *vn_len = vn_cnt; // �洢 vn_degree ����ĳ���
    } 
    else {
        printf("wrong mode!\n");
    }
}

// ���ݶȷֲ����������� R ����func�е���
// �ù�ʽ�������¡�The Optimized Scheme for FPGA-based  LDPC codes using Particle Swarm Optimization��
// ���޸�
double compute_code_rate(double rho[], double lambda[]) {    

    double sum_rho = 0.0, sum_lambda = 0.0;
    for (int j = 1; j <= cn_deg_max; j++) {  
        sum_rho += rho[j] * (double)j;    
    }   
    for (int i = 1; i <= vn_deg_max; i++) {
        sum_lambda += lambda[i] * (double)i;
    }

    // �������� R
    double R = 1.0 - (sum_lambda / sum_rho);
    return R;
}

// �����ض����ʶ�Ӧ��������׼���func�е���
double calculate_sigma_shannon(double R) {
    // ��������Ƿ�����Ч��Χ��
    if (R <= 0 || R >= 1) {
        printf("Invalid rate. R must be between 0 and 1.\n");
        return -1;  // ����һ�������ʶ
    }
    
    // ���ݹ�ʽ���� ��*
    double sigma_shannon = 1.0 / sqrt(2 * (pow(2, R) - 1));
    return sigma_shannon;
}

// ��ʧ���������㵱ǰ�ȷֲ��µ�������ֵ����ũ����������������ֵ�Ĳ�
// ֻ��Ҫ��ǰ����pop,�ú����ڲ����pop2rho/lambda�ͺ�������
double func(const double x[], double *Ecn, double *Evn, 
            double *vn_degree, double *cn_degree, 
            double *vn_edge_portion, double *cn_edge_portion, double sum_exp_theta_r, double sum_exp_theta_l) {
    
    update_rho(sum_exp_theta_r, x);
    update_lambda(sum_exp_theta_l, x);

    double cn_degree_buf[dimlimit]; // ��ʱ�洢У��ڵ�ȷֲ�
    double cn_edge_portion_buf[dimlimit]; // ��ʱ�洢У��ڵ�߱���
    double vn_degree_buf[dimlimit]; // ��ʱ�洢�����ڵ�ȷֲ�
    double vn_edge_portion_buf[dimlimit]; // ��ʱ�洢�����ڵ�߱���

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
    double shannon_limit = calculate_sigma_shannon(compute_code_rate(rho, lambda));   //���ʼ�����Ƶ���ũ����
  //  printf("shannon_limit = %f\n",shannon_limit);

    // Ŀ������С��SNR����ũ���޵Ĳ��,���ֵҪ������С����
   // printf("sigma_star = %f\n",fabs(snr_threshold - shannon_limit));
    return fabs(snr_threshold - shannon_limit);
}

// ��ʼ����Ⱥ���ٶȺ���Ӧ��
void initPopVFit(int sizePop, const double rangePop[2], const double rangeSpeed[2],
                 double pop[][dimlimit], double v[][dimlimit], double fitness[], double *Ecn, double *Evn, 
                double *vn_degree, double *cn_degree, 
                double *vn_edge_portion, double *cn_edge_portion) {
    
   //�����i������
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

        //Ϊ���������������ĳͷ�
        double R = compute_code_rate(rho, lambda);
        fitness[i] += (alpha_penalty * pow(fmax(0, R - pre_code_rate_limit), 2) + beta_penalty * pow(fmax(0, pred_code_rate_lowlimit - R), 2));
    }
}

// ��ȡ��ʼ����ֵ
void getInitBest(int sizePop, const double fitness[], const double pop[][dimlimit],
                 double gbestPop[], double *gbestFitness,
                 double pbestPop[][dimlimit], double pbestFitness[]) {
    int maxIdx = 0;                 
    for (int i = 0; i < sizePop; ++i) {
        if (fitness[i] < fitness[maxIdx]) {
            maxIdx = i;
        }
    }   //������һ�����������������ֵ
    
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

//PSO����
double getDeclineRate(int iter,int now){
    return ((double)iter-(double)now)/(double)iter;
}

// ����Ⱥ�Ż����º�����������ʧ����������ũ���޵Ĳ�ֵ��ԽСԽ��
void update_particles(int sizePop, double pop[][dimlimit], double v[][dimlimit], double fitness[],
                      double pbestPop[][dimlimit], double pbestFitness[], double gbestPop[], 
                      double *gbestFitness, int iter, int now_iter) {

    // pop[][dim]  �����������ӣ�ÿ����������dim���Ա����ĵ�ǰ��
    // v[][dim]  ����������ÿ���Ա����ϵ��ٶ�
    // fitness ÿ�����ӵ�������Ӧ��
    // pbestPop  ÿ����������dim���Ա��������Ž�
    // gbestPop  dim���Ա�����ȫ�����Ž�
    double lr[2];
    getLearningRate(lr);
    double rangeSpeed[2] = {-0.1, 0.1};   // �ٶȵ�������
    double rangePop[2] = {0, 1};   //λ�õ�������
    double sum_exp_theta_r[sizePop];
    double sum_exp_theta_l[sizePop];
    //ʹ��PSO��ʽ�����ٶ�
    for (int i = 0; i < sizePop; ++i) {
        for (int k = 0; k < dim; ++k) {
            if(k == 0 || k == 1 || k == cn_deg_max || k == cn_deg_max+1) v[i][k] = 0; //�����Ƕ���Ϊ0��1��  
            else{
                double w = getDeclineRate(iter,now_iter);
                v[i][k] += w * (lr[0] * ((double)rand() / RAND_MAX) * (pbestPop[i][k] - pop[i][k])
                        + lr[1] * ((double)rand() / RAND_MAX) * (gbestPop[k] - pop[i][k]));
                if (v[i][k] < rangeSpeed[0]) v[i][k] = rangeSpeed[0];
                if (v[i][k] > rangeSpeed[1]) v[i][k] = rangeSpeed[1];
            }
        }
    }
    
    //ʹ��PSO��ʽ����λ��
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
    
    // ������Ӧ�Ⱥ����Ž�
    for (int i = 0; i < sizePop; ++i) {

        //�����i�����ӵ���ʧ����
        fitness[i] = func(pop[i], Ecn, Evn, vn_degree, cn_degree, \
                          vn_edge_portion, cn_edge_portion, sum_exp_theta_r[i], sum_exp_theta_l[i]);
        
        //Ϊ���������������ĳͷ�
        double R = compute_code_rate(rho, lambda);
        fitness[i] += (alpha_penalty * pow(fmax(0, R - pre_code_rate_limit), 2) + beta_penalty * pow(fmax(0, pred_code_rate_lowlimit - R), 2));

        //�����������Ž�
        if (fitness[i] < pbestFitness[i]) {
            pbestFitness[i] = fitness[i];
            for(int k = 0; k < dim; ++k){
                pbestPop[i][k] = pop[i][k];
            }
        }
        //����ȫ�����Ž�
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