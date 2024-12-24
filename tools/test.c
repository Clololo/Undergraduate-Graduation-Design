#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "FDE.h"
#include "pso.h"

// #define PI 3.141592653589793
// #define MAX_ITER 1000     // ����������
// #define EPSILON 1e-6      // ����ֵ
// #define DV 6              // �������ڵ��
// #define DC 6              // ���У��ڵ��
// #define STEP_SIZE 0.1     // ������˹�任����
// #define SIGMA_TARGET 1.0  // Ŀ�������α� �ң��ɸ�����ũ���޵�����
// #define N 100             // ������Ϣ����

// // �ȷֲ����� (�� �� ��)
// double rho[DV + 1];  // �����ڵ�Ķȷֲ�����
// double lambda[DC + 1];  // У��ڵ�Ķȷֲ�����
// double messages[N];  // ������Ϣ����


int main() {
    srand(time(NULL));

    // ��ʼ���ȷֲ�
    initialize_degree_distributions();

    // ����Ⱥ�Ż��Ĳ���
    int sizePop = getSizePop();
    int maxGen = getMaxGen();
    double rangePop[2], rangeSpeed[2];
    getRangePop(rangePop);
    getRangeSpeed(rangeSpeed);

    // ����Ⱥ�Ż���ʼ��
    double pop[sizePop][2], v[sizePop][2], fitness[sizePop];
    initPopVFit(sizePop, rangePop, rangeSpeed, pop, v, fitness, Ecn, Evn, 
            vn_degree, cn_degree, vn_edge_portion, cn_edge_portion, 
            vn_len, cn_len);
    
    // ��ʼ���Ž�
    double gbestPop[2], gbestFitness;
    double pbestPop[sizePop][2], pbestFitness[sizePop];
    getInitBest(sizePop, fitness, pop, gbestPop, &gbestFitness, pbestPop, pbestFitness);

    int lastBest = -1;
    int repeCounter = 0;
    // ����Ⱥ�Ż���������
    for (int gen = 0; gen < maxGen; ++gen) {
        update_particles(sizePop, pop, v, fitness, pbestPop, pbestFitness, gbestPop, &gbestFitness);
        printf(" %d: Best Fitness = %f\n", gen, gbestFitness);
        // if(lastBest == gbestFitness){
        //     repeCounter++;
        //     if(repeCounter >= 100) break;
        // }
        // else{
        //     repeCounter = 0;
        // }
        // lastBest = gbestFitness;
    }

    // ����Ż���Ľ��
    printf("Optimized rho[2]: %f, lambda[2]: %f\n", gbestPop[0], gbestPop[1]);

    return 0;
}


