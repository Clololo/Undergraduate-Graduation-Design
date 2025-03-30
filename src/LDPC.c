#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "encoding/encode.h"
#include "newencoding/encode.h"
#include "newencoding/bg2read.h"
#include "decoding/decode.h"
#include "decoding/pso.h"
#include "trans/electron_sim.h"
#include "trans/AWGN.h"
#include "trans/trans.h"
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
double func(int particle_index, double alpha, double beta, int **H, int iteration, int num_frames, 
    double *pop, int M, int N, int g, int **Hs, int **Hp, double snr_db, double R) {

    double total_iters_with_pso = 0;
    double total_iters = 0;
    int success_frames = 0;

    for (int f = 0; f < num_frames; f++) {

        // ������Ϣλ
        int *randomS = (int *)malloc((N - M) * sizeof(double));    
        generateS(N - M, randomS);
        // ������
        int *C = (int *)malloc((N) * sizeof(double));
        Encoder2(Hs, Hp, randomS, bgm, bgn, bgz, C);
        // �������'
        printf("here");
        double *electron = (double*)malloc(N * sizeof(double));
        double *ctmp = (double*)malloc(N * sizeof(double));
        double *ctmp2 = (double*)malloc(N * sizeof(double));
        //printf("here");
        BPSK_Modulation(C, ctmp, N);
        //printf("here");
        // ����SNR��sigma
        double SNR_dB = run_snr_db + 10 * log10(R) - 10 * log10(0.5);
        double SNR_linear = pow(10.0, SNR_dB / 10.0);
        double sigma = sqrt(1.0 / SNR_linear);
        //printf("here");
        // AWGN�ŵ�
        AWGN_Channel(ctmp, ctmp2, N, SNR_dB, R);
        
        // ����˽���
        Receiver_LLR(ctmp2, electron, N, sigma);
        //for(int i = 0;i < N;i++) printf("%.1f ",electron[i]);
        //printf("\n");

        double *electron_test = (double*)malloc(codelength * sizeof(double));
        double *electron_compare = (double*)malloc(codelength * sizeof(double));
        memcpy(electron_test, electron, N * sizeof(double));
        memcpy(electron_compare, electron, N * sizeof(double));

        // ����

        int *resC = (int*)malloc(codelength * sizeof(int));
        int iters;
        int iters1;
        // //PSO�Ż���ĵ�������-->iters 
        // MINBP(C, electron, &resC, tanner, iteration, alpha, beta, &iters, pop);
        // //NORMAL�ĵ�������-->iters1
        // MINBP_NORMAL(C, electron, &resC, tanner, iteration, alpha, beta, &iters1);
        LDPCDecoder_NMS_WBP(H, electron, run_alpha, run_max_dc_iteration, M, N, resC, &iters, true, pop);
        LDPCDecoder_NMS_WBP(H, electron, run_alpha, run_max_dc_iteration, M, N, resC, &iters1, false, pop);

      //  printf("%d %d||", iters, iters1); 
        total_iters_with_pso += (double)iters;
        total_iters += (double)iters1;
       // compareDigit(resC, C_dup, N * Z);

        // �ͷ��ڴ�
        free(resC);
        free(C);
        free(ctmp);
        free(ctmp2);
        free(randomS);
        free(electron);
        free(electron_test);
        free(electron_compare);
    }

    printf("iter with pso: %.0f\n",total_iters_with_pso);        
    printf("iter without pso %.0f\n",total_iters);          
    return (total_iters - total_iters_with_pso) / (double)num_frames;    
}

void pso_optimize_min_sum(int **H, float alpha, float beta, int size, int codeLen, int decoding_time, int iteration, 
    double pop[][codelength], double v[][codelength], double fitness[],
    double gbestPop[], double *gbestFitness,
    double pbestPop[][codelength], double pbestFitness[],
    int N, int M, int g, int **Hs, int **Hp, double snr_db, double R) {
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
            fitness[i] = func(i, alpha, beta, H, iteration, update_interval, pop[i], M, N, g, Hs, Hp, snr_db, R);
           // printf("fitness = %f\n",fitness[i]);
            //printf("particle %d average opt-decoding %f times\n", i+1, fitness[i]);
        }
        // ��������Ⱥ
        update_particles(sizePop_de, codelength, pop, v, fitness, pbestPop, pbestFitness, gbestPop, gbestFitness);
    }

    // ѡȡ��Ѳ���
    getInitBest(sizePop_de, codelength, pop, v, fitness, gbestPop, gbestFitness, pbestPop, pbestFitness);
}

void run(int frames, double Eb_N0_dB, int iteration, float alpha, float beta){

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
    double R = 1.0 - (double)M/(double)N;  //����
    srand(time(NULL) + 1);  //Ϊ���������봴�����������

    //�������� end

    // // ʾ��ʹ
//     // ��������
    int mb = bgm, nb = bgn, z = bgz;
    int kb = nb - mb;
    
    int H_block[bgm][bgn];
    int bg_rows, bg_cols;
    
    read_base_graph("src\\bg2.csv", H_block, &bg_rows, &bg_cols);
    printf("Base graph loaded: %d x %d\n", bg_rows, bg_cols);

    int** H = (int**)malloc(mb*z * sizeof(int*));
    int** Hp = (int**)malloc(mb*z * sizeof(int*));
    int** Hs = (int**)malloc(mb*z * sizeof(int*));
    for (int i = 0; i < mb*z; i++) {
        H[i] = (int*)malloc(nb*z * sizeof(int));
        Hp[i] = (int*)malloc(mb*z * sizeof(int));
        Hs[i] = (int*)malloc(kb*z * sizeof(int));
    }
    
    HxMatrixGen(H_block, mb, nb, z, H, Hp, Hs);
    
    // ����׶� end
    // ����Ⱥ�Ż��Ĳ���
    
    //����Ⱥ�Ż���ʼ��
    double pop[sizePop_de][codelength], v[sizePop_de][codelength], fitness[sizePop_de];    
    initPopVFit(sizePop_de, codelength, pop, v, fitness);

    // ��ʼ���Ž�
    double gbestPop[codelength], gbestFitness;    
    double pbestPop[sizePop_de][codelength], pbestFitness[sizePop_de];
    //ʹ��PSO��Ȩ������ֵ��������
    pso_optimize_min_sum(H, alpha, beta, sizePop_de, codelength, frames, max_iteration, \
                         pop, v, fitness, gbestPop, &gbestFitness, pbestPop, pbestFitness, \
                         N*Z, M*Z, g, Hs, Hp, Eb_N0_dB, R);

    for(int i = 0; i < max_read_length; i++){
        printf("gbestPop[%d] = %f\n", i+1, gbestPop[i]);
    }
    printf("gbestFitness = %f\n",gbestFitness);

    //�����Ż�Ч��

    for(int i = 0; i < frames; i++){

        int *S = (int*)malloc(codelength * sizeof(int));
        generateS(Slen, S);
    
        double *ctmp = (double*)malloc(codelength * sizeof(double));
        double *ctmp2 = (double*)malloc(codelength * sizeof(double));
        double *electron = (double*)malloc(codelength * sizeof(double));
        int *resC_test = (int*)malloc(codelength * sizeof(int));
        int *resC_compare = (int*)malloc(codelength * sizeof(int));
        int *C_test = (int*)malloc(codelength * sizeof(int));
        int *C_compare = (int*)malloc(codelength * sizeof(int));
        int *C = (int*)malloc(codelength * sizeof(int));
        int *C_dup = (int*)malloc(codelength * sizeof(int));

        Encoder2(Hs, Hp, S, bgm, bgn, bgz, C);
        memcpy(C_dup, C,  N * Z * sizeof(int));

        // BPSK����
        BPSK_Modulation(C, ctmp, nz);

        // ����SNR��sigma
        double SNR_dB = Eb_N0_dB + 10 * log10(R) - 10 * log10(0.5);
        double SNR_linear = pow(10.0, SNR_dB / 10.0);
        double sigma = sqrt(1.0 / SNR_linear);
        
        // AWGN�ŵ�
        AWGN_Channel(ctmp, ctmp2, nz, SNR_dB, R);
        
        // ����˽���
        Receiver_LLR(ctmp2, electron, nz, sigma);


        double *electron_test = (double*)malloc(codelength * sizeof(double));
        double *electron_compare = (double*)malloc(codelength * sizeof(double));
        memcpy(electron_test, electron, nz * sizeof(double));
        memcpy(electron_compare, electron, nz * sizeof(double));

        int iter_test = 0;
        int iter_compare = 0;

        LDPCDecoder_NMS_WBP(H, electron_test, run_alpha, run_max_dc_iteration, mz, nz, C_test, &iter_test, true, gbestPop);
        LDPCDecoder_NMS_WBP(H, electron_compare, run_alpha, run_max_dc_iteration, mz, nz, C_compare, &iter_compare, false, gbestPop);
        
        int testerrorBit = compareDigit(C_test, C_dup, nz);
        int compareerrorBit = compareDigit(C_compare, C_dup, nz);

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
        free(S);
    }

    freeMatrix(H, mz);  // free H2D

    errorFrame /= 2;  //����ʵ����Ͷ������������

    float simulateBER = (double)((double)errorBit / (double)allbit);
    float trueBER = (double)((double)errorBitWithLDPC / (double)allbit);

    //float simulateFER = (double)((double)errorFrame / (double)frames);
   // float trueFER = (double)((double)errorFrameWithLDPC / (double)frames);
    
    printf("-----------------------------------------------------\n\n");
    printf("%d %d %lld\n",errorBit,errorBitWithLDPC,allbit);
    printf("bits transmitted: %d frames transmitted: %d\n",allbit, frames);
    printf("WRONG bits in transmission: %d\n",errorBit);
    printf("WRONG bits after LDPC: %d\n",errorBitWithLDPC);
    printf("WRONG bits after LDPC(compare): %d\n", errorBitWithLDPCCompare);
   // printf("WRONG frames in transmission: %d\n",errorFrame);
   // printf("WRONG frames after LDPC: %d\n",errorFrameWithLDPC);
   // printf("WRONG frames after LDPC(compare): %d\n", errorFrameWithLDPCCompare);
    printf("total iter:%d\n",countTest);
    printf("total iter(compare):%d\n",countCompare);
    printf("The simulated BER is %e\nunder LDPC, the actual BER is %e\n", simulateBER, trueBER);
   // printf("The simulated FER is %e\nunder LDPC, the actual FER is %e\n", simulateFER, trueFER);

    printHorizontalLine(WIDTH, '|', '=');
    
    printRow(WIDTH, " K/N    ALPHA/BETA    Bits   Frames  ITERS   BER     BER_LDPC    FER     FER_LDPC", " ");
    
    // Parameters params = {
    //     .int1 = K * Z, .int2 = N * Z,
    //     .float1 = alpha, .float2 = beta,
    //     .int3 = allbit, .int4 = frames, .int5 = iteration,
    //     .float4 = simulateBER, .float5 = trueBER,
    //     .float6 = simulateFER, .float7 = trueFER
    // };

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
  //  fprintf(file, "%.2f, %.2f,%d,%d,%d,%d,%.2e,%.2e,%.2e,%.2e,%.1f\n", alpha, beta, snr_db, allbit, frames, iteration, simulateBER, trueBER, simulateFER, trueFER, elapsed_time/10);

    // �ر��ļ�
    fclose(file);
}


