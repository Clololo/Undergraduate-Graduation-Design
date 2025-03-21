#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "encoding/encode.h"
#include "decoding/decode.h"
#include "decoding/pso.h"
#include "trans/electron_sim.h"
#include "trans/AWGN.h"
#include "tools/utils.h"
#include "tools/mat_tool.h"
#include "tools/print_tool.h"
#include "preH.h"


int errorBit = 0;  //记录误码实际发生的位数
int errorBitWithLDPC = 0;  //记录误码无法纠正的位数
int errorFrame = 0;
int errorFrameWithLDPC = 0;
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

//return 优化掉的迭代次数
double func(int particle_index, double alpha, double beta, Graph* tanner, int iteration, int num_frames, 
    double *pop, int N, int M, int Z, int g, int **CCT, int **DDT_inv, int **AT, int **BT, int **TT_inv,
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
        matrixMultiply(S, CCT, &p11, 1, (N-M)*Z, g);
        matrixMultiply(p11, DDT_inv, &p1, 1, g, g);

        int **p21, **p22, **p23;
        matrixMultiply(S, AT, &p21, 1, (N-M)*Z, M*Z - g);
        matrixMultiply(p1, BT, &p22, 1, g, M*Z - g);
        matrixAdd(p21, p22, &p23, 1, M*Z - g);
        matrixMultiply(p23, TT_inv, &p2, 1, M*Z - g, M*Z - g);
        // 生成码字
        int *C, *C_dup;

        fillArrayWithFirstRow(S, p1, p2, &C, (N-M)*Z, g, M * Z - g);
        fillArrayWithFirstRow(S, p1, p2, &C_dup, (N-M)*Z, g, M * Z - g);
        // generateDigit(C2D, N * Z, &C);
        // generateDigit(C2D, N * Z, &C_dup);   //生成矩阵 得到码字
        //printDigit(C, N*Z);
        if(ct(C,tanner))
        {
            iter_success ++;
        }

        // 添加噪声
        double *electron;
        awgn_channel(C, &electron, N * Z, snr_db);
        changeDigit(&C, electron, N * Z);

        // 计算误码率
        for (int i = 0; i < N * Z; i++) {
            if (C_dup[i] == 0 && electron[i] > 0 || C_dup[i] == 1 && electron[i] < 0) {
                errorBit++;
            }
        }

        // 译码
        int *resC;
        int iters;
        int iters1;

        //PSO优化后的迭代次数-->iters 
        MINBP(C, electron, &resC, tanner, iteration, alpha, beta, &iters, pop);
        //NORMAL的迭代次数-->iters1
        MINBP_NORMAL(C, electron, &resC, tanner, iteration, alpha, beta, &iters1);

      //  printf("%d %d||", iters, iters1); 
        total_iters_with_pso += (double)iters;
        total_iters += (double)iters1;
        compareDigit(resC, C_dup, N * Z);

        // 释放内存
        free(resC);
        free(C);
        free(C_dup);
        freeMatrix(S, 1);
    }

    printf("iter with pso: %.0f\n",total_iters_with_pso);        
    printf("iter without pso %.0f\n",total_iters);          
    return (total_iters - total_iters_with_pso) / (double)num_frames;    
}

void pso_optimize_min_sum(Graph* tanner, float alpha, float beta, int size, int codeLen, int decoding_time, int iteration, 
    double pop[][codelength], double v[][codelength], double fitness[],
    double gbestPop[], double *gbestFitness,
    double pbestPop[][codelength], double pbestFitness[],
    int N, int M, int Z, int g, int **CCT, int **DDT_inv, int **AT, int **BT, int **TT_inv, int Slen, double snr_db) {
    // 初始化 PSO 参数
    initPopVFit(sizePop, codeLen, pop, v, fitness);
    getInitBest(sizePop, codeLen, pop, v, fitness, gbestPop, gbestFitness, pbestPop, pbestFitness);

    // 进行 PSO 优化
    // 总解码次数  decoding_time次，前decoding_time *0.2次用于PSO优化
    int optimization_iters = decoding_time * opt_rate;

    //更新窗口为100
    int update_interval = update_window;
    for (int iter = 0; iter < optimization_iters; iter += update_interval) {
        //  每个粒子单独用于译码
        for (int i = 0; i < sizePop; i++) {
            // == 每个func译码 update_interval次
            //在这里fitness已经更新，不需要再到update_particles更新fitness 
            fitness[i] = func(i, alpha, beta, tanner, iteration, update_interval, pop[i], N, M, Z, g, CCT, DDT_inv, AT, BT, TT_inv, Slen, snr_db);
           // printf("fitness = %f\n",fitness[i]);
            //printf("particle %d average opt-decoding %f times\n", i+1, fitness[i]);
        }
        // 更新粒子群
        update_particles(sizePop, codelength, pop, v, fitness, pbestPop, pbestFitness, gbestPop, gbestFitness);
    }

    // 选取最佳参数
    getInitBest(sizePop, codelength, pop, v, fitness, gbestPop, gbestFitness, pbestPop, pbestFitness);
}


void run(int frames, int snr_db, int iteration, float alpha, float beta){
    errorBit = 0;  //记录误码实际发生的位数
    errorBitWithLDPC = 0;  //记录误码无法纠正的位数
    errorFrame = 0;
    errorFrameWithLDPC = 0;
    iter_success = 0;
    clock_t start_time = clock();
    //参数设置 begin
    int iter_num = frames;
    int M = 4;  //  校验节点个数
    int N = 8;  // 编码后的码字位 （变量节点个数）
    int Z = 4;  // Size of each submatrix (Z x Z)
    int g = 4;  // g 称为近似表示的 gap，g 越小，编码复杂度越低
    int K = N - M; // 信息位
    int Slen = (N - M) * Z;  // 编码后的信息位
    long long allbit = (frames * N * Z);

    srand(time(NULL) + 1);  //为后续的误码创建随机数种子


    //参数设置 end

    int ***H;
    int **H2D,**A,**B,**T,**C,**D,**E,**CC,**DD;
    Graph* tanner;

    //编码阶段 begin

    // generateQCLDPCMatrix(M, N, Z, &H);
    // convertTo2DMatrix(M, N, Z, H, &H2D);
    // printMatrix(H2D, M * Z, N * Z);
    copyPreMat_16_32(preH_16_32, &H2D, M * Z, N * Z);
    tanner = constructTannerGraph(M * Z, N * Z, H2D);
    MatrixDivide(&H2D, &A, &B, &T, &C, &D, &E, M*Z, N*Z, g);

    // printQCLDPCMatrix(M, N, Z, H);
    // printf("Generated 2D QC-LDPC H matrix:\n");
    // printMatrix(H2D, M * Z, N * Z);
    // printGraph(tanner);   //在高斯消元之前得到tanner图
    

    int **T_inv;
    matrix_inverse(T, &T_inv, M * Z - g);

    calculateResult(C, E, T_inv, A, &CC, g, M * Z - g, M * Z - g, (N-M)*Z);

    calculateResult(D, E, T_inv, B, &DD, g, M * Z - g, M * Z - g, g);

    int **CCT,**DDT,**DDT_inv,**AT,**BT,**TT, **TT_inv;
    transposeMatrix(CC, &CCT, g, (N-M)*Z);

    transposeMatrix(DD, &DDT, g, g);

    matrix_inverse(DDT, &DDT_inv, g);

    transposeMatrix(A, &AT, M * Z - g, (N - M)*Z); 

    transposeMatrix(B, &BT, M * Z - g, g);

    transposeMatrix(T, &TT, M * Z - g, M * Z - g);

    matrix_inverse(TT, &TT_inv, M * Z - g);

    /*
    //这是使用生成矩阵的方式
    int **G;
    
    getGeneratorMatrix(M * Z, N * Z, H2D, &G);
    printf("\n");
    printf("Generated 2D QC-LDPC G matrix:\n");
    printMatrix(G, (N - M) * Z, N * Z);
    */
    
    // 编码阶段 end
    // 粒子群优化的参数
    double rangePop[2], rangeSpeed[2];
    getRangePop(rangePop);
    getRangeSpeed(rangeSpeed);
    
    //粒子群优化初始化
    double pop[sizePop][codelength], v[sizePop][codelength], fitness[sizePop];    
    initPopVFit(sizePop, codelength, pop, v, fitness);

    // 初始最优解
    double gbestPop[sizePop], gbestFitness;    
    double pbestPop[sizePop][codelength], pbestFitness[sizePop];

    pso_optimize_min_sum(tanner, alpha, beta, sizePop, codelength, frames, max_iteration, \
                         pop, v, fitness, gbestPop, &gbestFitness, pbestPop, pbestFitness, \
                         N, M, Z, g, CCT, DDT_inv, AT, BT, TT_inv, Slen, snr_db);

    for(int i = 0; i < Slen; i++){
        printf("gbestPop[%d] = %f\n", i+1, gbestPop[i]);
    }
    printf("gbestFitness = %f\n",gbestFitness);

    //  freeQCLDPCMatrix(M, N, Z, H);   //free H
    freeMatrix(H2D, M * Z);  // free H2D
    //freeMatrix(G, M * Z);  //free G
    freeGraph(tanner);  //free tanner
    // freeQCLDPCMatrix(M, N, Z, H);   
    freeMatrix(A, M * Z - g);  
    freeMatrix(B, M * Z - g);  
    freeMatrix(C, g);  
    freeMatrix(D, g);  
    freeMatrix(E, g);  
    freeMatrix(T, M * Z - g);  

    float simulateBER = (double)((double)errorBit / (double)allbit);
    float trueBER = (double)((double)errorBitWithLDPC / (double)allbit);

    float simulateFER = (double)((double)errorFrame / (double)frames);
    float trueFER = (double)((double)errorFrameWithLDPC / (double)frames);
    
//     printf("-----------------------------------------------------\n\n");
   // printf("%d %d %lld\n",errorBit,errorBitWithLDPC,allbit);
    // printf("bits transmitted: %d frames transmitted: %d\n",allbit, frames);
    // printf("WRONG bits in transmission: %d\n",errorBit);
    // printf("WRONG bits after LDPC: %d\n",errorBitWithLDPC);
    // printf("WRONG frames in transmission: %d\n",errorFrame);
    // printf("WRONG frames after LDPC: %d\n",errorFrameWithLDPC);
    // printf("The simulated BER is %e\nunder LDPC, the actual BER is %e\n", simulateBER, trueBER);
    // printf("The simulated FER is %e\nunder LDPC, the actual FER is %e\n", simulateFER, trueFER);

    // printHorizontalLine(WIDTH, '|', '=');
    
    //printRow(WIDTH, " K/N    ALPHA/BETA    Bits   Frames  ITERS   BER     BER_LDPC    FER     FER_LDPC", " ");
    
    Parameters params = {
        .int1 = K*Z, .int2 = N*Z,
        .float1 = alpha, .float2 = beta,
        .int3 = allbit, .int4 = frames, .int5 = iteration,
        .float4 = simulateBER, .float5 = trueBER,
        .float6 = simulateFER, .float7 = trueFER
    };

    // 打印格式化的数字行
   // printFormattedLine(params, WIDTH);
    // 打印方框的底部边界
   // printHorizontalLine(WIDTH, '|', '=');
    clock_t end_time = clock();
    
    // 计算运行时间
    double elapsed_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;
    
    // 打印运行时间
    printf("total running time: %.1f ms\n", elapsed_time*1000);
    
    FILE *file = fopen("record/result_with_ieee.csv", "a");
    if (file == NULL) {
        perror("无法打开文件");
        return;
    }

    // 将值写入文件
    fprintf(file, "%.2f, %.2f,%d,%d,%d,%d,%.2e,%.2e,%.2e,%.2e,%.1f\n", alpha, beta, snr_db, allbit, frames, iteration, simulateBER, trueBER, simulateFER, trueFER, elapsed_time/10);

    // 关闭文件
    fclose(file);
}


