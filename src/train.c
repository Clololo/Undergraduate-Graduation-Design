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


//return 优化掉的迭代次数
//每一次func意味着调用num_frames次generateRandomBinaryString，即完成编码num_frames次编译码过程
double func(int particle_index, double alpha, double beta, int **H, int iteration, int num_frames, 
    double *pop, int M, int N, int g, int **Hs, int **Hp, double Eb_N0_dB, double R) {

    double total_iters_with_pso = 0.0;
    double total_iters = 0.0;
    int success_frames = 0;
    double total_errors_with_pso = 0.0;
    double total_errors = 0.0;

    for (int f = 0; f < num_frames; f++) {

        // 生成信息位
        int *randomS = (int *)malloc((N - M) * sizeof(double));    
        generateS(N - M, randomS);

        // 生成码
        int *C = (int *)malloc((N) * sizeof(double));
        Encoder(H, Hs, Hp, randomS, bgm, bgn - bgm, bgz, C);

        // 添加噪声'
        double *electron = (double*)malloc(N * sizeof(double));
        double *ctmp = (double*)malloc(N * sizeof(double));
        double *ctmp2 = (double*)malloc(N * sizeof(double));
        BPSK_Modulation(C, ctmp, N);
        // 计算SNR和sigma
        double SNR_dB = Eb_N0_dB + 10 * log10(R) - 10 * log10(0.5);
        double SNR_linear = pow(10.0, SNR_dB / 10.0);
        double sigma = sqrt(1.0 / SNR_linear);
        // AWGN信道
        AWGN_Channel(ctmp, ctmp2, N, SNR_dB, R);
        
        // 译码端接收
        Receiver_LLR(ctmp2, electron, N, sigma);
        //for(int i = 0;i < N;i++) printf("%.1f ",electron[i]);
        //printf("\n");

        double *electron_test = (double*)malloc(codelength * sizeof(double));
        double *electron_compare = (double*)malloc(codelength * sizeof(double));
        memcpy(electron_test, electron, N * sizeof(double));
        memcpy(electron_compare, electron, N * sizeof(double));

        // 译码

        int *resC = (int*)malloc(codelength * sizeof(int));
        int iters;
        int iters1;
        int error1 = 0;
        int error2 = 0;
        // //PSO优化后的迭代次数-->iters 
        // MINBP(C, electron, &resC, tanner, iteration, alpha, beta, &iters, pop);
        // //NORMAL的迭代次数-->iters1
        // MINBP_NORMAL(C, electron, &resC, tanner, iteration, alpha, beta, &iters1);
        LDPCDecoder(H, electron, alpha, run_max_dc_iteration, M, N, resC, &iters, &error1, true, pop);
        LDPCDecoder(H, electron, alpha, run_max_dc_iteration, M, N, resC, &iters1, &error2, false, pop);


        total_iters_with_pso += (double)iters;
        total_iters += (double)iters1;
        total_errors_with_pso += error1;
        total_errors += error2;

        // 释放内存
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

    // 进行 PSO 优化
    int optimization_iters = opt_time;
    int update_interval = update_window;
    for (int iter = 0; iter < optimization_iters; iter += update_interval) {
        //  每个粒子单独用于译码   
        if((iter*10)%optimization_iters == 0 && iter!= 0) printf("%d%% opt-finished\n",(iter*100/optimization_iters));
        // for(int i = 0; i < codelength; i++ ){
        //     printf("POP[%d] = %f\n",i,pop[0][i]);
        // }
        for (int i = 0; i < sizePop_de; i++) {
            // == 每个func译码 update_interval次
            //在这里fitness已经更新，不需要再到update_particles更新fitness 
            fitness[i] = func(i, alpha, beta, H, iteration, update_interval, pop[i], M, N, g, Hs, Hp, Eb_N0_dB, R);
           // printf("fitness = %f\n",fitness[i]);
            //printf("particle %d average opt-decoding %f times\n", i+1, fitness[i]);
        }
        // 更新粒子群
        update_particles(sizePop_de, codelength, pop, v, fitness, pbestPop, pbestFitness, gbestPop, gbestFitness);
    }

    // 选取最佳参数
    getInitBest(sizePop_de, codelength, pop, v, fitness, gbestPop, gbestFitness, pbestPop, pbestFitness);
}


void train(double SNR){
    clock_t start_time = clock();
    //参数设置 begin
    int M = checknodeNum;  // 校验节点个数
    int N = SymbolnodeNum;  // 编码后的码字位 （变量节点个数）
    int Z = SubmatrixSize;  // Size of each submatrix (Z x Z)
    int g = gap;  // g 称为近似表示的 gap，g 越小，编码复杂度越低
    int K = N - M; // 信息位
    int Slen = (N - M) * Z;  // 编码后的信息位
    int mz = M * Z;   
    int nz = N * Z;   //实际码长
    double R = 1.0 - (double)M/(double)N;  //码率
    srand(time(NULL) + 1);  //为后续的误码创建随机数种子
    //参数设置 end
    // // 示例使
//     // 参数配置
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
    
    //通过定义的H_block得到校验矩阵
    if(!usepsogenH) {
        read_base_graph("src/bg2.csv", H_block, &bg_rows, &bg_cols);
        printf("Base graph loaded: %d x %d\n", bg_rows, bg_cols);
        HGenerator(H_block, mb, nb, z, H, Hp, Hs);
    }
    //通过PSO得到优化的校验矩阵
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

    // 编码阶段 end
    // 粒子群优化的参数
    // 粒子群优化初始化
    double pop[sizePop_de][codelength], v[sizePop_de][codelength], fitness[sizePop_de];    
    initPopVFit(sizePop_de, codelength, pop, v, fitness);

    // 初始最优解
    double gbestPop[codelength], gbestFitness;    
    double pbestPop[sizePop_de][codelength], pbestFitness[sizePop_de];
    //使用PSO对权重最优值进行搜索
    initPopVFit(sizePop_de, codelength, pop, v, fitness);
    getInitBest(sizePop_de, codelength, pop, v, fitness, gbestPop, &gbestFitness, pbestPop, pbestFitness);

    pso_optimize_min_sum(H, run_alpha, run_beta, sizePop_de, codelength, opt_time, max_iteration, \
                         pop, v, fitness, gbestPop, &gbestFitness, pbestPop, pbestFitness, \
                         N*Z, M*Z, g, Hs, Hp, SNR, R);
    
    clock_t end_time = clock();

        // 计算运行时间
    double elapsed_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;
    
    save_pop_to_csv(SNR, gbestPop, codelength, usepsogenH);
    
        // 打印运行时间
    printf("SNR = %.1fdB, training time: %.1f ms\n", SNR, elapsed_time * 1000);

}

int main(){
    clock_t start1_time = clock();
    for(double snr = 0.0; snr <= 5.0; snr += 0.5){
        train(snr);
    }
    clock_t end1_time = clock();

    // 计算运行时间
    double elapsed1_time = (double)(end1_time - start1_time) / CLOCKS_PER_SEC;    

    // 打印运行时间
    printf("total training time: %.1f s\n", elapsed1_time);
}