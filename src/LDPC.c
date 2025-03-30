#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include "encoding/encode.h"
#include "decoding/decode.h"
#include "decoding/pso.h"
#include "trans/electron_sim.h"
#include "trans/AWGN.h"
#include "tools/utils.h"
#include "tools/mat_tool.h"
#include "tools/print_tool.h"
#include "preH.h"
#include "config.h"

int errorBit = 0;  //��¼����ʵ�ʷ�����λ��
int errorBitWithLDPC = 0;  //��¼�����޷�������λ��(PSO�Ż�)
int errorBitWithLDPCCompare = 0; //��¼�����޷�������λ������PSO�Ż����գ�
int errorFrame = 0;   //��¼�����޷�������֡��
int errorFrameWithLDPC = 0;  //��¼�����޷�������֡��(PSO�Ż�)
int errorFrameWithLDPCCompare = 0;  //��¼�����޷�������֡������PSO�Ż����գ�
int countTest = 0;   //��¼��С���ܵ�������(PSO�Ż�)
int countCompare = 0;   //��¼��С���ܵ�����������PSO�Ż����գ�
int iter_success = 0;
int iters_tmp = 0;

int ct(int *C, Graph* tanner) {
  for (int i = 0; i < tanner->numC; i++) {
        int sum = 0;
        AdjListNode* pC = tanner->array[i].head;
        while (pC != NULL) {
            int j = pC->dest;
            sum ^= C[j - tanner->numC];
            pC = pC->next;
        }
        if (sum) return 0;  // ����δͨ��
    }
    return 1;  // ����ͨ��
}

//return �Ż����ĵ�������
//ÿһ��func��ζ�ŵ���num_frames��generateRandomBinaryString������ɱ���num_frames�α��������
double func(int particle_index, double alpha, double beta, Graph* tanner, int iteration, int num_frames, 
    double *pop, int M, int N, int g, int **CCT, int **DDT_inv, int **AT, int **BT, int **TT_inv,
    int Slen, double snr_db) {

    double total_iters_with_pso = 0;
    double total_iters = 0;
    int success_frames = 0;

    for (int f = 0; f < num_frames; f++) {
        int **S;
        generateRandomBinaryString(Slen, &S);
        int **C2D;
        int **p1;
        int **p2;

        int **p11;
        matrixMultiply(S, CCT, &p11, 1, (N - M), g);
        matrixMultiply(p11, DDT_inv, &p1, 1, g, g);

        int **p21, **p22, **p23;
        matrixMultiply(S, AT, &p21, 1, (N - M), M - g);
        matrixMultiply(p1, BT, &p22, 1, g, M - g);
        matrixAdd(p21, p22, &p23, 1, M - g);
        matrixMultiply(p23, TT_inv, &p2, 1, M - g, M - g);

        // ��������
        int *C, *C_dup;
        fillArrayWithFirstRow(S, p1, p2, &C, (N - M), g, M - g);
        fillArrayWithFirstRow(S, p1, p2, &C_dup, (N - M), g, M - g);
        // generateDigit(C2D, N * Z, &C);
        // generateDigit(C2D, N * Z, &C_dup);   //���ɾ��� �õ�����
        //printDigit(C, N * Z);
        if(ct(C,tanner))
        {
            iter_success ++;
        }

        // �������
        double *electron;
        awgn_channel(C, &electron, N, snr_db);
        changeDigit(&C, electron, N);

        // ����

        int *resC;
        int iters;
        int iters1;
        //PSO�Ż���ĵ�������-->iters 
        MINBP(C, electron, &resC, tanner, iteration, alpha, beta, &iters, pop);
        //NORMAL�ĵ�������-->iters1
        MINBP_NORMAL(C, electron, &resC, tanner, iteration, alpha, beta, &iters1);

      //  printf("%d %d||", iters, iters1); 
        total_iters_with_pso += (double)iters;
        total_iters += (double)iters1;
       // compareDigit(resC, C_dup, N * Z);

        // �ͷ��ڴ�
        free(resC);
        free(C);
        free(C_dup);
        freeMatrix(S, 1);
    }

//    printf("iter with pso: %.0f\n",total_iters_with_pso);        
//    printf("iter without pso %.0f\n",total_iters);          
    return (total_iters - total_iters_with_pso) / (double)num_frames;    
}

void pso_optimize_min_sum(Graph* tanner, float alpha, float beta, int size, int codeLen, int decoding_time, int iteration, 
    double pop[][codelength], double v[][codelength], double fitness[],
    double gbestPop[], double *gbestFitness,
    double pbestPop[][codelength], double pbestFitness[],
    int N, int M, int Z, int g, int **CCT, int **DDT_inv, int **AT, int **BT, int **TT_inv, int Slen, double snr_db) {
    // ��ʼ�� PSO ����
    initPopVFit(sizePop_de, codeLen, pop, v, fitness);
    getInitBest(sizePop_de, codeLen, pop, v, fitness, gbestPop, gbestFitness, pbestPop, pbestFitness);

    // ���� PSO �Ż�
    // �ܽ������  decoding_time�Σ�ǰdecoding_time *0.2������PSO�Ż�
    int optimization_iters = (int)((double)decoding_time * opt_rate);

    //���´���Ϊ100
    int update_interval = update_window;
    for (int iter = 0; iter < optimization_iters; iter += update_interval) {
        //  ÿ�����ӵ�����������
        for (int i = 0; i < sizePop_de; i++) {
            // == ÿ��func���� update_interval��
            //������fitness�Ѿ����£�����Ҫ�ٵ�update_particles����fitness 
            fitness[i] = func(i, alpha, beta, tanner, iteration, update_interval, pop[i], M * Z, N * Z, g, CCT, DDT_inv, AT, BT, TT_inv, Slen, snr_db);
           // printf("fitness = %f\n",fitness[i]);
            //printf("particle %d average opt-decoding %f times\n", i+1, fitness[i]);
        }
        // ��������Ⱥ
        update_particles(sizePop_de, codelength, pop, v, fitness, pbestPop, pbestFitness, gbestPop, gbestFitness);
    }

    // ѡȡ��Ѳ���
    getInitBest(sizePop_de, codelength, pop, v, fitness, gbestPop, gbestFitness, pbestPop, pbestFitness);
}

void run(int frames, int snr_db, int iteration, float alpha, float beta){

    iter_success = 0;
    clock_t start_time = clock();
    //�������� begin
    int iter_num = frames;
    int M = checknodeNum;  // У��ڵ����
    int N = SymbolnodeNum;  // ����������λ �������ڵ������
    int Z = SubmatrixSize;  // Size of each submatrix (Z x Z)
    int g = gap;  // g ��Ϊ���Ʊ�ʾ�� gap��g ԽС�����븴�Ӷ�Խ��
    int K = N - M; // ��Ϣλ
    int Slen = (N - M) * Z;  // ��������Ϣλ
    long long allbit = (frames * N * Z);
    int mz = M * Z;   
    int nz = N * Z;   //ʵ���볤
    srand(time(NULL) + 1);  //Ϊ���������봴�����������

    //�������� end

    int ***H;
    int **H2D,**A,**B,**T,**C,**D,**E,**CC,**DD;
    Graph* tanner;
    //����׶� begin
    //ѡ��У�����//5G��׼����orPSO�Ż��ľ���
    //copyPreMat_16_32(preH_16_32, &H2D, mz, nz);   
    copyPSOMat("./output/optim_checkmatrix.csv", &H2D, mz, nz);

    tanner = constructTannerGraph(mz, nz, H2D);
    MatrixDivide(&H2D, &A, &B, &T, &C, &D, &E, mz, nz, g);
    int **T_inv;
    matrix_inverse(T, &T_inv, mz - g);
    calculateResult(C, E, T_inv, A, &CC, g, mz - g, mz - g, nz - mz);
    calculateResult(D, E, T_inv, B, &DD, g, mz - g, mz - g, g);
    int **CCT,**DDT,**DDT_inv,**AT,**BT,**TT, **TT_inv;
    transposeMatrix(CC, &CCT, g, nz - mz);
    transposeMatrix(DD, &DDT, g, g);
    matrix_inverse(DDT, &DDT_inv, g);
    transposeMatrix(A, &AT, mz - g, nz - mz); 
    transposeMatrix(B, &BT, mz - g, g);
    transposeMatrix(T, &TT, mz - g, mz - g);
    matrix_inverse(TT, &TT_inv, mz - g);

    /*
    //����ʹ�����ɾ���ķ�ʽ
    int **G;
    getGeneratorMatrix(mz, N * Z, H2D, &G);
    printMatrix(G, (N - M) * Z, N * Z);
    */
    
    // ����׶� end
    // ����Ⱥ�Ż��Ĳ���
    
    //����Ⱥ�Ż���ʼ��
    double pop[sizePop_de][codelength], v[sizePop_de][codelength], fitness[sizePop_de];    
    initPopVFit(sizePop_de, codelength, pop, v, fitness);

    // ��ʼ���Ž�
    double gbestPop[codelength], gbestFitness;    
    double pbestPop[sizePop_de][codelength], pbestFitness[sizePop_de];

    //ʹ��PSO��Ȩ������ֵ��������
    pso_optimize_min_sum(tanner, alpha, beta, sizePop_de, codelength, frames, max_iteration, \
                         pop, v, fitness, gbestPop, &gbestFitness, pbestPop, pbestFitness, \
                         N, M, Z, g, CCT, DDT_inv, AT, BT, TT_inv, Slen, snr_db);

    for(int i = 0; i < max_read_length; i++){
        printf("gbestPop[%d] = %f\n", i+1, gbestPop[i]);
    }
    printf("gbestFitness = %f\n",gbestFitness);

    //�����Ż�Ч��

    for(int i = 0; i < frames; i++){

        int **S;
        generateRandomBinaryString(Slen, &S);
    
        // ����У��λ p1 �� p2
        int **p1, **p2;
        int **p11 = NULL;
        matrixMultiply(S, CCT, &p11, 1, (N - M)*Z, g);
        matrixMultiply(p11, DDT_inv, &p1, 1, g, g);
    
        int **p21 = NULL, **p22 = NULL, **p23 = NULL;
        matrixMultiply(S, AT, &p21, 1, (N - M)*Z, mz - g);
        matrixMultiply(p1, BT, &p22, 1, g, mz - g);
        matrixAdd(p21, p22, &p23, 1, mz - g);
        matrixMultiply(p23, TT_inv, &p2, 1, mz - g, mz - g);
    
        // ����ԭʼ���� C ����� C_dup
        int *C = NULL, *C_dup = NULL;
        fillArrayWithFirstRow(S, p1, p2, &C, (N - M)*Z, g, mz - g);
        fillArrayWithFirstRow(S, p1, p2, &C_dup, (N - M)*Z, g, mz - g);
        // ������������ electron ���ݣ�

        double *electron = (double*)malloc(codelength * sizeof(double));
        int *resC_test = (int*)malloc(codelength * sizeof(int));
        int *resC_compare = (int*)malloc(codelength * sizeof(int));
        int *C_test = (int*)malloc(codelength * sizeof(int));
        int *C_compare = (int*)malloc(codelength * sizeof(int));


        awgn_channel(C, &electron, nz, snr_db);
        changeDigit(&C_test, electron, nz);
        changeDigit(&C_compare, electron, nz);


        double *electron_test = (double*)malloc(codelength * sizeof(double));
        double *electron_compare = (double*)malloc(codelength * sizeof(double));
        memcpy(electron_test, electron, nz * sizeof(double));
        memcpy(electron_compare, electron, nz * sizeof(double));

        double allone[codelength];
        for(int i = 0; i < codelength; i++){
            allone[i] = 1.0;
        }

        // ����ԭʼ������
        for (int i = 0; i < nz; i++) {
            if (C_dup[i] == 0 && electron[i] > 0 || C_dup[i] == 1 && electron[i] < 0) {
                errorBit++;
            }
        }

        int iter_test = 0;
        int iter_compare = 0;

        //����ֵΪ0�������ʧ�ܣ�����֡+1
        errorFrameWithLDPC += 1^MINBP_TEST_COMPARE(C_test, electron_test, &resC_test, tanner, iteration, alpha, beta, &iter_test, gbestPop);
        errorFrameWithLDPCCompare += 1^MINBP_TEST_COMPARE(C_compare, electron_compare, &resC_compare, tanner, iteration, alpha, beta, &iter_compare, allone);
        
        int testerrorBit = compareDigit(resC_test, C_dup, nz);
        int compareerrorBit = compareDigit(resC_compare, C_dup, nz);

        errorBitWithLDPC += testerrorBit;
        errorBitWithLDPCCompare += compareerrorBit;

        countTest += iter_test;
        countCompare += iter_compare;

        free(C);
        free(C_dup);
        free(resC_test);
        free(resC_compare);
        free(C_test);
        free(C_compare);
        free(electron);

        freeMatrix(S, 1);
        freeMatrix(p11, 1);
        freeMatrix(p21, 1);
        freeMatrix(p22, 1);
        freeMatrix(p23, 1);
        freeMatrix(p1, 1);
        freeMatrix(p2, 1);
    }

    //  freeQCLDPCMatrix(M, N, Z, H);   //free H
    freeMatrix(H2D, mz);  // free H2D
    //freeMatrix(G, mz);  //free G
    freeGraph(tanner);  //free tanner
    // freeQCLDPCMatrix(M, N, Z, H);   
    freeMatrix(A, mz - g);  
    freeMatrix(B, mz - g);  
    freeMatrix(C, g);  
    freeMatrix(D, g);  
    freeMatrix(E, g);  
    freeMatrix(T, mz - g);  

    errorFrame /= 2;  //����ʵ����Ͷ������������

    float simulateBER = (double)((double)errorBit / (double)allbit);
    float trueBER = (double)((double)errorBitWithLDPC / (double)allbit);

    float simulateFER = (double)((double)errorFrame / (double)frames);
    float trueFER = (double)((double)errorFrameWithLDPC / (double)frames);
    
    printf("-----------------------------------------------------\n\n");
    printf("%d %d %lld\n",errorBit,errorBitWithLDPC,allbit);
    printf("bits transmitted: %d frames transmitted: %d\n",allbit, frames);
    printf("WRONG bits in transmission: %d\n",errorBit);
    printf("WRONG bits after LDPC: %d\n",errorBitWithLDPC);
    printf("WRONG bits after LDPC(compare): %d\n", errorBitWithLDPCCompare);
    printf("WRONG frames in transmission: %d\n",errorFrame);
    printf("WRONG frames after LDPC: %d\n",errorFrameWithLDPC);
    printf("WRONG frames after LDPC(compare): %d\n", errorFrameWithLDPCCompare);
    printf("total iter:%d\n",countTest);
    printf("total iter(compare):%d\n",countCompare);
    printf("The simulated BER is %e\nunder LDPC, the actual BER is %e\n", simulateBER, trueBER);
    printf("The simulated FER is %e\nunder LDPC, the actual FER is %e\n", simulateFER, trueFER);

    printHorizontalLine(WIDTH, '|', '=');
    
    printRow(WIDTH, " K/N    ALPHA/BETA    Bits   Frames  ITERS   BER     BER_LDPC    FER     FER_LDPC", " ");
    
    Parameters params = {
        .int1 = K * Z, .int2 = N * Z,
        .float1 = alpha, .float2 = beta,
        .int3 = allbit, .int4 = frames, .int5 = iteration,
        .float4 = simulateBER, .float5 = trueBER,
        .float6 = simulateFER, .float7 = trueFER
    };

    // ��ӡ��ʽ����������
    // printFormattedLine(params, WIDTH);
    // ��ӡ����ĵײ��߽�
    // printHorizontalLine(WIDTH, '|', '=');
    clock_t end_time = clock();
    
    // ��������ʱ��
    double elapsed_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;
    
    // ��ӡ����ʱ��
    printf("total running time: %.1f ms\n", elapsed_time * 1000);
    
    FILE *file = fopen("record/result_with_ieee.csv", "a");
    if (file == NULL) { 
        perror("�޷����ļ�");
        return;
    }

    // ��ֵд���ļ�
    fprintf(file, "%.2f, %.2f,%d,%d,%d,%d,%.2e,%.2e,%.2e,%.2e,%.1f\n", alpha, beta, snr_db, allbit, frames, iteration, simulateBER, trueBER, simulateFER, trueFER, elapsed_time/10);

    // �ر��ļ�
    fclose(file);
}


