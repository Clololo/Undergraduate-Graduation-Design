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

double rho[dimlimit];  // �����ڵ�Ķȷֲ�����
double lambda[dimlimit];  // У��ڵ�Ķȷֲ�����
double messages[dimlimit];  // ������Ϣ����

// ��ȡ����Ȩ��
double getWeight() {
    return 1.0;
}

// ��ȡѧϰ����
void getLearningRate(double lr[2]) {
    lr[0] = 0.49445;
    lr[1] = 1.49445;
}

// ����λ�÷�Χ���Զ�����
void getRangePop(double rangePop[2]) {
    rangePop[0] = 0.0;
    rangePop[1] = 1.0;
}

// �����ٶȷ�Χ
void getRangeSpeed(double rangeSpeed[2]) {
    rangeSpeed[0] = -0.2;
    rangeSpeed[1] = 0.2;
}

//��ʼ��ʹ��ʼ����ȫΪ2.
void initialize_degree_distributions() {    
    for (int i = 2; i <= vn_l; i++) {     
        rho[i] = (i == 2) ? 1.0 : 0.0;  // ֻ�ж�Ϊ2�ı����ڵ��и���
    }   
    for (int i = 3; i <= cn_l; i++) { 
        lambda[i] = (i == 2) ? 1.0 : 0.0;  // ֻ�ж�Ϊ3��У��ڵ��и���
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
 
//��һ������
void normalize_distribution(double *dist, int len) {   
    double sum = 0.0;
    for (int i = 0; i < len; i++) {
        sum += dist[i];
    }
    for (int i = 0; i < len; i++) {
        dist[i] /= sum;
    }
}

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
    for (int i = 0; i < len; i++) {
        if (pos[i] > 0) {
            cn_degree[cn_cnt] = i;
            printf("cn_degree[%d] = %d\n", cn_cnt, i);
            cn_edge_portion[cn_cnt] = pos[i];
            cn_cnt++;
        }
    }
    *cn_len = cn_cnt; // �洢 cn_degree ����ĳ���
    } else if (mode == 2) {
    // ����У��ڵ㣨CN������������ڵ㣨VN���������
        for (int i = 0; i < len; i++) {
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
double func(const double x[], double *Ecn, double *Evn, 
            double *vn_degree, double *cn_degree, 
            double *vn_edge_portion, double *cn_edge_portion, 
            int vn_len, int cn_len) {
    
    // ��������������ȡУ��ڵ�ȷֲ�
    for (int i = 0; i < cn_deg_max; i++) {
        rho[i] = x[i];  
    }

    // ��������������ȡ�����ڵ�ȷֲ�
    for (int i = cn_deg_max,j = 0; i < cn_deg_max+vn_deg_max; i++,j++) {
        lambda[j] = x[i];  
    }

    normalize_distribution(rho, cn_len);
    normalize_distribution(lambda, vn_len);


    int vn_deg_len = 0;
    int cn_deg_len = 0;

    double cn_degree_buf[dimlimit]; // ��ʱ�洢У��ڵ�ȷֲ�
    double cn_edge_portion_buf[dimlimit]; // ��ʱ�洢У��ڵ�߱���
    double vn_degree_buf[dimlimit]; // ��ʱ�洢�����ڵ�ȷֲ�
    double vn_edge_portion_buf[dimlimit]; // ��ʱ�洢�����ڵ�߱���

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
    // ʹ���ŵ������е��߼�������SNR
    // int vn_deg_len = cn_l / 10;
    // int cn_deg_len = vn_l / 10;

    double snr_threshold = iterative_snr_threshold(SIGMA_TARGET, 1, Ecn, Evn, 
                                                   vn_degree, cn_degree, 
                                                   vn_edge_portion, cn_edge_portion, 
                                                   vn_deg_len, cn_deg_len);
    
    printf("snr_threshold = %f\n",snr_threshold);
    double shannon_limit = calculate_sigma_shannon(compute_code_rate());   //���ʼ�����Ƶ���ũ����
    printf("shannon_limit = %f\n",shannon_limit);

    // Ŀ������С��SNR����ũ���޵Ĳ��,���ֵҪ������С����
    return fabs(snr_threshold - shannon_limit);
}

// ��ʼ����Ⱥ���ٶȺ���Ӧ��
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

    //�����ٶ�
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

    //����λ��
    for (int i = 0; i < sizePop; ++i) {
        for (int k = 0; k < dim; ++k) {
            pop[i][k] += v[i][k];   
            if (pop[i][k] < rangePop[0]) pop[i][k] = rangePop[0];
            if (pop[i][k] > rangePop[1]) pop[i][k] = rangePop[1];
        }
    }
    
    // ������Ӧ�Ⱥ����Ž�
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