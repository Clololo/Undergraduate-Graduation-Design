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


void run(int frames, double Eb_N0_dB, int iteration, float alpha, float beta, bool UsePSOGenH){
    //��������
    errorBit = 0;  //��¼����ʵ�ʷ�����λ��
    errorBitWithLDPC = 0;  //��¼�����޷�������λ��(PSO�Ż�)
    errorBitWithLDPCCompare = 0; //��¼�����޷�������λ������PSO�Ż����գ�
    errorFrame = 0;   //��¼�����޷�������֡��
    errorFrameWithLDPC = 0;  //��¼�����޷�������֡��(PSO�Ż�)
    errorFrameWithLDPCCompare = 0;  //��¼�����޷�������֡������PSO�Ż����գ�
    iter_success = 0;
    countTest = 0;
    countCompare = 0;

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

    int** H = (int**)malloc(mb*z * sizeof(int*));
    int** Hp = (int**)malloc(mb*z * sizeof(int*));
    int** Hs = (int**)malloc(mb*z * sizeof(int*));
    for (int i = 0; i < mb*z; i++) {
        H[i] = (int*)malloc(nb*z * sizeof(int));
        Hp[i] = (int*)malloc(mb*z * sizeof(int));
        Hs[i] = (int*)malloc(kb*z * sizeof(int));
    }
    
    //ͨ�������H_block�õ�У�����
    if(!UsePSOGenH) {
        read_base_graph("src/bg2.csv", H_block, &bg_rows, &bg_cols);
        printf("Base graph loaded: %d x %d\n", bg_rows, bg_cols);
        HGenerator(H_block, mb, nb, z, H, Hp, Hs);
    }
    //ͨ��PSO�õ��Ż���У�����
    else{
        PSOHGenerator("output/optim_checkmatrix.csv", mz, nz, H, Hp, Hs);
        int H_fullrank = is_full_rank(H, mz, nz);
        int Hp_fullrank = is_full_rank(Hp, mz, mz);
        int Hs_fullrank = is_full_rank(Hs, mz, nz-mz);
        if(H_fullrank && Hp_fullrank && Hs_fullrank){
            printf("Generate valid Matrix\n");
        }
        else{
            printf("Generate INVALID Matrix!!! info:%d %d %d\n",H_fullrank, Hp_fullrank, Hs_fullrank);
            return;
        }

        // for(int i = 0; i < mz; i++){
        //     for(int j = 0; j < nz ;j++){
        //         printf("%d",H[i][j]);
        //     }
        //     printf("\n");
        // }
        // printf("----------------------------\n");
        // for(int i = 0; i < mz; i++){
        //     for(int j = 0; j < mz ;j++){
        //         printf("%d",Hp[i][j]);
        //     }
        //     printf("\n");
        // }
        // printf("----------------------------\n");
        // for(int i = 0; i < mz; i++){
        //     for(int j = 0; j < nz - mz ;j++){
        //         printf("%d",Hs[i][j]);
        //     }
        //     printf("\n");
        // }
        // printf("----------------------------\n");
    }

    // ����׶� end
    // ����Ⱥ�Ż��Ĳ���
    // ����Ⱥ�Ż���ʼ��
    //double pop[sizePop_de][codelength], v[sizePop_de][codelength], fitness[sizePop_de];    
    //initPopVFit(sizePop_de, codelength, pop, v, fitness);

    // ��ʼ���Ž�
    double gbestPop[codelength], gbestFitness;    
    //double pbestPop[sizePop_de][codelength], pbestFitness[sizePop_de];
    //ʹ��PSO��Ȩ������ֵ��������
    // initPopVFit(sizePop_de, codelength, pop, v, fitness);
    // getInitBest(sizePop_de, codelength, pop, v, fitness, gbestPop, &gbestFitness, pbestPop, pbestFitness);
    // pso_optimize_min_sum(H, alpha, beta, sizePop_de, codelength, frames, max_iteration, \
    //                      pop, v, fitness, gbestPop, &gbestFitness, pbestPop, pbestFitness, \
    //                      N*Z, M*Z, g, Hs, Hp, Eb_N0_dB, R);

    //ֱ�Ӷ�ȡԤѵ����POPȨ��
    load_pop_from_csv(Eb_N0_dB, gbestPop, codelength, usepsogenH);
    // for(int i = 0; i < max_read_length; i++){
    //     printf("gbestPop[%d] = %f\n", i+1, gbestPop[i]);
    // }
    // printf("gbestFitness = %f\n",gbestFitness);

    //�����Ż�Ч��

    for(int i = 0; i < frames; i++){
        if((i*10)%frames == 0 && i!= 0) printf("%d%% finished\n",(i*100/frames));
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

        Encoder(H, Hs, Hp, S, bgm, bgn - bgm, bgz, C);


        memcpy(C_dup, C,  N * Z * sizeof(int));
       

        // BPSK����
        BPSK_Modulation(C, ctmp, nz);

        // ����SNR��sigma
        double SNR_dB = Eb_N0_dB + 10 * log10(R) - 10 * log10(0.5);
        double SNR_linear = pow(10.0, SNR_dB / 10.0);
        double sigma = sqrt(1.0 / SNR_linear);
        
        //printf("sigma = %f\n",sigma);
        // AWGN�ŵ�
        AWGN_Channel(ctmp, ctmp2, codelength, sigma, R);
        
        // ����˽���
        Receiver_LLR(ctmp2, electron, codelength, sigma);
        // for(int i = 0; i < 10;i++) printf("%.3f ",ctmp[i]);
        // printf("\n");
        // for(int i = 0; i < 10;i++) printf("%.3f ",ctmp2[i]);
        // printf("\n");
        // printf("-----------------\n");
        double *electron_test = (double*)malloc(codelength * sizeof(double));
        double *electron_compare = (double*)malloc(codelength * sizeof(double));
        memcpy(electron_test, electron, nz * sizeof(double));
        memcpy(electron_compare, electron, nz * sizeof(double));

        int iter_test = 0;
        int iter_compare = 0;

        int error1 = 0;

        LDPCDecoder(H, electron_test, alpha, run_max_dc_iteration, mz, nz, C_test, &iter_test, &error1, true, gbestPop);
        LDPCDecoder(H, electron_compare, alpha, run_max_dc_iteration, mz, nz, C_compare, &iter_compare, &error1, false, gbestPop);
        
        int testerrorBit = compareDigit(C_test, C_dup, nz);
        int compareerrorBit = compareDigit(C_compare, C_dup, nz);

        errorBitWithLDPC += testerrorBit;
        errorBitWithLDPCCompare += compareerrorBit;

        if(testerrorBit != 0) errorFrameWithLDPC ++;
        if(compareerrorBit != 0) errorFrameWithLDPCCompare ++;

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

    //float simulateBER = (double)((double)errorBit / (double)allbit);
    float trueBER = (double)((double)errorBitWithLDPC / (double)allbit);
    float trueBERcompare = (double)((double)errorBitWithLDPCCompare / (double)allbit);
    float trueFER = (double)((double)errorFrameWithLDPC / (double)frames);
    float trueFERcompare = (double)((double)errorFrameWithLDPCCompare / (double)frames);

    //float simulateFER = (double)((double)errorFrame / (double)frames);
   // float trueFER = (double)((double)errorFrameWithLDPC / (double)frames);
    
    printf("-----------------------------------------------------\n\n");
    printf("%.2f %d %lld\n", Eb_N0_dB, frames, allbit);
    printf("bits transmitted: %d frames transmitted: %d\n",allbit, frames);
    //printf("WRONG bits in transmission: %d\n",errorBit);
    printf("WRONG bits after LDPC: %d\n",errorBitWithLDPC);
    printf("WRONG bits after LDPC(compare): %d\n", errorBitWithLDPCCompare);
   // printf("WRONG frames in transmission: %d\n",errorFrame);
    printf("WRONG frames after LDPC: %d\n",errorFrameWithLDPC);
    printf("WRONG frames after LDPC(compare): %d\n", errorFrameWithLDPCCompare);
    printf("AVG total iter:%.2f\n",(double)countTest/(double)frames);
    printf("AVG total iter(compare):%.2f\n",(double)countCompare/(double)frames);
    printf("The OPT BER is %e, NOR BER is %e\n", trueBER, trueBERcompare);
    printf("The OPT FER is %e, NOR FER is %e\n", trueFER, trueFERcompare);
   // printf("The simulated FER is %e\nunder LDPC, the actual FER is %e\n", simulateFER, trueFER);

    // printHorizontalLine(WIDTH, '|', '=');
    
    // printRow(WIDTH, " K/N    ALPHA/BETA    Bits   Frames  ITERS   BER     BER_LDPC    FER     FER_LDPC", " ");
    
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

    char filename_store[64];
    snprintf(filename_store, sizeof(filename_store), "output/er_results_%d_%d_%d.csv", codelength, (int)(R*100), usepsogenH);
    append_performance_data(filename_store, Eb_N0_dB, trueBER, trueBERcompare, trueFER, trueFERcompare, (double)countTest/(double)frames, (double)countCompare/(double)frames);

    // ��ӡ����ʱ��
    printf("SNR = %.1fdB, running time: %.1f ms\n", Eb_N0_dB, elapsed_time * 1000);
    
    // ��ֵд���ļ�
    // fprintf(file, "%.2f, %.2f,%d,%d,%d,%d,%.2e,%.2e,%.2e,%.2e,%.1f\n", alpha, beta, snr_db, allbit, frames, iteration, simulateBER, trueBER, simulateFER, trueFER, elapsed_time/10);
    // �ر��ļ�
}


