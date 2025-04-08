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

int errorBit = 0;  //记录误码实际发生的位数
int errorBitWithLDPC = 0;  //记录误码无法纠正的位数(PSO优化)
int errorBitWithLDPCCompare = 0; //记录误码无法纠正的位数（无PSO优化对照）
int errorFrame = 0;   //记录误码无法纠正的帧数
int errorFrameWithLDPC = 0;  //记录误码无法纠正的帧数(PSO优化)
int errorFrameWithLDPCCompare = 0;  //记录误码无法纠正的帧数（无PSO优化对照）
int countTest = 0;   //记录最小和总迭代次数(PSO优化)
int countCompare = 0;   //记录最小和总迭代次数（无PSO优化对照）
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
        if (sum) return 0;  // 检验未通过
    }
    return 1;  // 检验通过
}


void run(int frames, double Eb_N0_dB, int iteration, float alpha, float beta, bool UsePSOGenH){
    //重置数据
    errorBit = 0;  //记录误码实际发生的位数
    errorBitWithLDPC = 0;  //记录误码无法纠正的位数(PSO优化)
    errorBitWithLDPCCompare = 0; //记录误码无法纠正的位数（无PSO优化对照）
    errorFrame = 0;   //记录误码无法纠正的帧数
    errorFrameWithLDPC = 0;  //记录误码无法纠正的帧数(PSO优化)
    errorFrameWithLDPCCompare = 0;  //记录误码无法纠正的帧数（无PSO优化对照）
    iter_success = 0;
    countTest = 0;
    countCompare = 0;

    clock_t start_time = clock();
    //参数设置 begin
    int iter_num = frames;
    int M = checknodeNum;  // 校验节点个数
    int N = SymbolnodeNum;  // 编码后的码字位 （变量节点个数）
    int Z = SubmatrixSize;  // Size of each submatrix (Z x Z)
    int g = gap;  // g 称为近似表示的 gap，g 越小，编码复杂度越低
    int K = N - M; // 信息位
    int Slen = (N - M) * Z;  // 编码后的信息位
    long long allbit = (frames * N * Z);
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
    if(!UsePSOGenH) {
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

    // 编码阶段 end
    // 粒子群优化的参数
    // 粒子群优化初始化
    //double pop[sizePop_de][codelength], v[sizePop_de][codelength], fitness[sizePop_de];    
    //initPopVFit(sizePop_de, codelength, pop, v, fitness);

    // 初始最优解
    double gbestPop[codelength], gbestFitness;    
    //double pbestPop[sizePop_de][codelength], pbestFitness[sizePop_de];
    //使用PSO对权重最优值进行搜索
    // initPopVFit(sizePop_de, codelength, pop, v, fitness);
    // getInitBest(sizePop_de, codelength, pop, v, fitness, gbestPop, &gbestFitness, pbestPop, pbestFitness);
    // pso_optimize_min_sum(H, alpha, beta, sizePop_de, codelength, frames, max_iteration, \
    //                      pop, v, fitness, gbestPop, &gbestFitness, pbestPop, pbestFitness, \
    //                      N*Z, M*Z, g, Hs, Hp, Eb_N0_dB, R);

    //直接读取预训练的POP权重
    load_pop_from_csv(Eb_N0_dB, gbestPop, codelength, usepsogenH);
    // for(int i = 0; i < max_read_length; i++){
    //     printf("gbestPop[%d] = %f\n", i+1, gbestPop[i]);
    // }
    // printf("gbestFitness = %f\n",gbestFitness);

    //测试优化效果

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
       

        // BPSK调制
        BPSK_Modulation(C, ctmp, nz);

        // 计算SNR和sigma
        double SNR_dB = Eb_N0_dB + 10 * log10(R) - 10 * log10(0.5);
        double SNR_linear = pow(10.0, SNR_dB / 10.0);
        double sigma = sqrt(1.0 / SNR_linear);
        
        //printf("sigma = %f\n",sigma);
        // AWGN信道
        AWGN_Channel(ctmp, ctmp2, codelength, sigma, R);
        
        // 译码端接收
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

    errorFrame /= 2;  //调用实验组和对照组加了两次

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

    // 打印格式化的数字行
    // printFormattedLine(params, WIDTH);
    // 打印方框的底部边界
    // printHorizontalLine(WIDTH, '|', '=');

    
    clock_t end_time = clock();
    
    // 计算运行时间
    double elapsed_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;

    char filename_store[64];
    snprintf(filename_store, sizeof(filename_store), "output/er_results_%d_%d_%d.csv", codelength, (int)(R*100), usepsogenH);
    append_performance_data(filename_store, Eb_N0_dB, trueBER, trueBERcompare, trueFER, trueFERcompare, (double)countTest/(double)frames, (double)countCompare/(double)frames);

    // 打印运行时间
    printf("SNR = %.1fdB, running time: %.1f ms\n", Eb_N0_dB, elapsed_time * 1000);
    
    // 将值写入文件
    // fprintf(file, "%.2f, %.2f,%d,%d,%d,%d,%.2e,%.2e,%.2e,%.2e,%.1f\n", alpha, beta, snr_db, allbit, frames, iteration, simulateBER, trueBER, simulateFER, trueFER, elapsed_time/10);
    // 关闭文件
}


