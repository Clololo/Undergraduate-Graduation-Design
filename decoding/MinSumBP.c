#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "decode.h"
#include "../encoding/encode.h"

extern int errorFrame;
extern int errorFrameWithLDPC;

// �����յ�������ת��ΪLLR
void convert_to_llr(double **llr, double *data, int N) {
    for (int i = 0; i < N; i++) {
        (*llr)[i] = data[i];
    }
}

// ��ʼ����Ϣ
void initialize_messages(double ***C2V, double ***V2C, double *llr, Graph* tanner) {
    for (int i = 0; i < tanner->numC; i++) {
        AdjListNode* pC = tanner->array[i].head;
        while (pC != NULL) {
            int j = pC->dest;
            (*C2V)[i][j - tanner->numC] = 0.0;
            (*V2C)[i][j - tanner->numC] = llr[j - tanner->numC];
            pC = pC->next;
        }
    }
}

// ���򴫲�
void horizontal_step(double ***C2V, double ***V2C, Graph* tanner, float alpha, float beta) {
    for(int i = 0; i < tanner->numC; i++){  //����У��ڵ�
        AdjListNode* pC1 = tanner->array[i].head;   //��i��У��ڵ������ĵ�һ�������ڵ�
        while (pC1 != NULL) {
            int j = pC1->dest;   //j�ǵ�i��У��ڵ�������ĳһ�������ڵ�
            double min_val = INFINITY;
            int sign_product = 1;
            AdjListNode* pC2 = tanner->array[i].head;  //��У��ڵ������ĵ�һ�������ڵ�
            while(pC2 != NULL){
                int k = pC2->dest;  //k�ǵ�i��У��ڵ�������ĳһ�������ڵ�
                if(k != j){
                    double val = fabs((*V2C)[i][k - tanner->numC]);
                    if (val < min_val){
                        min_val = val;
                    }
                    if ((*V2C)[i][k - tanner->numC] < 0){
                        sign_product *= -1;
                    }
                }
                pC2 = pC2->next;
            }
            (*C2V)[i][j - tanner->numC] = alpha * sign_product * fmax(0, min_val - beta) ;
            pC1 = pC1->next;
        }
    }
}

// ���򴫲�
void vertical_step(double ***C2V, double ***V2C, double *llr, Graph* tanner) {
    for(int j = tanner->numC; j < tanner->numNodes; j++){
        AdjListNode* pV = tanner->array[j].head;  //ѡ��һ�������ڵ�
        while (pV != NULL) {
            int i = pV->dest;  //��֮������һ��У��ڵ�
            double message = llr[j - tanner->numC];
                        //double message = 0;

            AdjListNode* pC = tanner->array[j].head;
            while (pC != NULL) {
                int k = pC->dest;
                if (k != i) {
                    message += (*C2V)[k][j - tanner->numC];
                }
                pC = pC->next;
            }
            (*V2C)[i][j - tanner->numC] = message;
            pV = pV->next;
        }
    }
}

// ����
void decision(int **decode_C, double **C2V, double *data, Graph* tanner) {
    for (int j = tanner->numC; j < tanner->numNodes; j++) {
        double sum = 0;
        AdjListNode* pC = tanner->array[j].head;
        while (pC != NULL) {
            int i = pC->dest;
            sum += C2V[i][j - tanner->numC];
            pC = pC->next;
        }
        sum += data[j - tanner->numC];
        (*decode_C)[j - tanner->numC] = (sum >= 0) ? 1 : 0;
    }
}

// У���Ƿ����ɹ�
int check_syndrome(int *decode_C, Graph* tanner) {
    for (int i = 0; i < tanner->numC; i++) {
        int sum = 0;
        AdjListNode* pC = tanner->array[i].head;
        while (pC != NULL) {
            int j = pC->dest;
            sum ^= (int)decode_C[j - tanner->numC];
            pC = pC->next;
        }
        if (sum) return 0;  // ����δͨ��
    }
    return 1;  // ����ͨ��
}

int str;

bool MINBP(int *C, double *data, int **resC, Graph* tanner, int MAX_ITERATIONS, float alpha, float beta) {
    *resC = (int *)malloc(tanner->numV * sizeof(int));
    if (check_syndrome((int *)C, tanner)){
        //printf("no problem\n");
        for (int j = 0; j < tanner->numV; j++) {
            (*resC)[j] = C[j];
        }
      //  printf("right\n");
        return 1;
    }
    errorFrame += 1;  //ûͨ����ʼ��⣬˵����һ֡������
    // printf("wrong!\n");
//    for(int i = 0;i<tanner->numV;i++){
//        printf("%d",C[i]);
//    }
//    printf("\n");
    int *decode_C = (int *)malloc(tanner->numV * sizeof(int));
    double *llr = (double *)malloc(tanner->numV * sizeof(double));

    // ������Ϣ����
    double **C2V = (double **)malloc(tanner->numC * sizeof(double *));
    double **V2C = (double **)malloc(tanner->numC * sizeof(double *));
    for (int i = 0; i < tanner->numC; i++) {
        C2V[i] = (double *)malloc(tanner->numV * sizeof(double));
        V2C[i] = (double *)malloc(tanner->numV * sizeof(double));
    }

    // �����յ�������ת��ΪLLR
    convert_to_llr(&llr, data, tanner->numV);

    // ��ʼ����Ϣ
    initialize_messages(&C2V, &V2C, llr, tanner);
    int iter = 0;
    bool flag = false;
    for (iter = 0; iter < MAX_ITERATIONS; iter++) {
        horizontal_step(&C2V, &V2C, tanner, alpha, beta);
        vertical_step(&C2V, &V2C, llr, tanner);
        decision(&decode_C, C2V, data, tanner);
        if (check_syndrome(decode_C, tanner)) {    // �Ѿ�����
             //printf(" success after iter:%d become", iter + 1);
            // for(int i = 0;i<tanner->numV;i++){
            //     printf("%d",decode_C[i]);
            // }
            // printf("\n");
            flag = true;
            break;
        }
    }
    if(iter < MAX_ITERATIONS)
    {
        for (int j = 0; j < tanner->numV; j++) {
            (*resC)[j] = decode_C[j];
        }
   }
    else{
        for (int j = 0; j < tanner->numV; j++) {
            (*resC)[j] = C[j];
        }
        //printf("failed\n");
    }
    for (int i = 0; i < tanner->numC; i++) {
        free(C2V[i]);
        free(V2C[i]);
    }
    free(C2V);
    free(V2C);
    free(decode_C);
    free(llr);
    errorFrameWithLDPC += (flag == 1)? 0 : 1;
    return flag;
}

