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


//return �Ż����ĵ�������
//ÿһ��func��ζ�ŵ���num_frames��generateRandomBinaryString������ɱ���num_frames�α��������
double func(int particle_index, double alpha, double beta, int **H, int iteration, int num_frames, 
    double *pop, int M, int N, int g, int **Hs, int **Hp, double Eb_N0_dB, double R) {

    double total_iters_with_pso = 0.0;
    double total_iters = 0.0;
    int success_frames = 0;
    double total_errors_with_pso = 0.0;
    double total_errors = 0.0;

    for (int f = 0; f < num_frames; f++) {

        // ������Ϣλ
        int *randomS = (int *)malloc((N - M) * sizeof(double));    
        generateS(N - M, randomS);

        // ������
        int *C = (int *)malloc((N) * sizeof(double));
        Encoder(H, Hs, Hp, randomS, bgm, bgn - bgm, bgz, C);

        // �������'
        double *electron = (double*)malloc(N * sizeof(double));
        double *ctmp = (double*)malloc(N * sizeof(double));
        double *ctmp2 = (double*)malloc(N * sizeof(double));
        BPSK_Modulation(C, ctmp, N);
        // ����SNR��sigma
        double SNR_dB = Eb_N0_dB + 10 * log10(R) - 10 * log10(0.5);
        double SNR_linear = pow(10.0, SNR_dB / 10.0);
        double sigma = sqrt(1.0 / SNR_linear);
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
        int error1 = 0;
        int error2 = 0;
        // //PSO�Ż���ĵ�������-->iters 
        // MINBP(C, electron, &resC, tanner, iteration, alpha, beta, &iters, pop);
        // //NORMAL�ĵ�������-->iters1
        // MINBP_NORMAL(C, electron, &resC, tanner, iteration, alpha, beta, &iters1);
        LDPCDecoder(H, electron, alpha, run_max_dc_iteration, M, N, resC, &iters, &error1, true, pop);
        LDPCDecoder(H, electron, alpha, run_max_dc_iteration, M, N, resC, &iters1, &error2, false, pop);


        total_iters_with_pso += (double)iters;
        total_iters += (double)iters1;
        total_errors_with_pso += error1;
        total_errors += error2;

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

    //printf("error with pso: %.0f\n",total_errors_with_pso);        
    //printf("error without pso %.0f\n",total_errors);          
    return (total_iters - total_iters_with_pso) / (double)num_frames;   
    //printf("func num = %f\n",(total_errors - total_errors_with_pso) / (double)num_frames);
    //return (total_errors - total_errors_with_pso) / (double)num_frames;   
}

void pso_optimize_min_sum(int **H, float alpha, float beta, int size, int codeLen, int decoding_time, int iteration, 
    double pop[][codelength], double v[][codelength], double fitness[],
    double gbestPop[], double *gbestFitness,
    double pbestPop[][codelength], double pbestFitness[],
    int N, int M, int g, int **Hs, int **Hp, double Eb_N0_dB, double R) {

    // ���� PSO �Ż�
    int optimization_iters = opt_time;
    int update_interval = update_window;
    for (int iter = 0; iter < optimization_iters; iter += update_interval) {
        //  ÿ�����ӵ�����������   
        if((iter*10)%optimization_iters == 0 && iter!= 0) printf("%d%% opt-finished\n",(iter*100/optimization_iters));
        // for(int i = 0; i < codelength; i++ ){
        //     printf("POP[%d] = %f\n",i,pop[0][i]);
        // }
        for (int i = 0; i < sizePop_de; i++) {
            // == ÿ��func���� update_interval��
            //������fitness�Ѿ����£�����Ҫ�ٵ�update_particles����fitness 
            fitness[i] = func(i, alpha, beta, H, iteration, update_interval, pop[i], M, N, g, Hs, Hp, Eb_N0_dB, R);
           // printf("fitness = %f\n",fitness[i]);
            //printf("particle %d average opt-decoding %f times\n", i+1, fitness[i]);
        }
        // ��������Ⱥ
        update_particles(sizePop_de, codelength, pop, v, fitness, pbestPop, pbestFitness, gbestPop, gbestFitness);
    }

    // ѡȡ��Ѳ���
    getInitBest(sizePop_de, codelength, pop, v, fitness, gbestPop, gbestFitness, pbestPop, pbestFitness);
}


void train(double SNR){
    clock_t start_time = clock();
    //�������� begin
    int M = checknodeNum;  // У��ڵ����
    int N = SymbolnodeNum;  // ����������λ �������ڵ������
    int Z = SubmatrixSize;  // Size of each submatrix (Z x Z)
    int g = gap;  // g ��Ϊ���Ʊ�ʾ�� gap��g ԽС�����븴�Ӷ�Խ��
    int K = N - M; // ��Ϣλ
    int Slen = (N - M) * Z;  // ��������Ϣλ
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
    if(!usepsogenH) {
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
    }

    // ����׶� end
    // ����Ⱥ�Ż��Ĳ���
    // ����Ⱥ�Ż���ʼ��
    double pop[sizePop_de][codelength], v[sizePop_de][codelength], fitness[sizePop_de];    
    initPopVFit(sizePop_de, codelength, pop, v, fitness);

    // ��ʼ���Ž�
    double gbestPop[codelength], gbestFitness;    
    double pbestPop[sizePop_de][codelength], pbestFitness[sizePop_de];
    //ʹ��PSO��Ȩ������ֵ��������
    initPopVFit(sizePop_de, codelength, pop, v, fitness);
    getInitBest(sizePop_de, codelength, pop, v, fitness, gbestPop, &gbestFitness, pbestPop, pbestFitness);

    pso_optimize_min_sum(H, run_alpha, run_beta, sizePop_de, codelength, opt_time, max_iteration, \
                         pop, v, fitness, gbestPop, &gbestFitness, pbestPop, pbestFitness, \
                         N*Z, M*Z, g, Hs, Hp, SNR, R);
    
    clock_t end_time = clock();

        // ��������ʱ��
    double elapsed_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;
    
    save_pop_to_csv(SNR, gbestPop, codelength, usepsogenH);
    
        // ��ӡ����ʱ��
    printf("SNR = %.1fdB, training time: %.1f ms\n", SNR, elapsed_time * 1000);

}

int main(){
    clock_t start1_time = clock();
    for(double snr = 0.0; snr <= 5.0; snr += 0.5){
        train(snr);
    }
    clock_t end1_time = clock();

    // ��������ʱ��
    double elapsed1_time = (double)(end1_time - start1_time) / CLOCKS_PER_SEC;    

    // ��ӡ����ʱ��
    printf("total training time: %.1f s\n", elapsed1_time);
}