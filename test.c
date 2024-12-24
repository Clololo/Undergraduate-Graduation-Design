#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "encoding/encode.h"
#include "decoding/decode.h"
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


    for(int i = 1;i<=frames;i++)
    {
        //printf("----------------------------\n");
        // 数据传输阶段 begin
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

        //matrixMultiply(S, G, &C2D, 1, Slen, N*Z);    //生成矩阵 矩阵乘得到码字C

        int *C;   
        int *C_dup;
        fillArrayWithFirstRow(S, p1, p2, &C, (N-M)*Z, g, M * Z - g);
        fillArrayWithFirstRow(S, p1, p2, &C_dup, (N-M)*Z, g, M * Z - g);
        // generateDigit(C2D, N * Z, &C);
        // generateDigit(C2D, N * Z, &C_dup);   //生成矩阵 得到码字
        //printDigit(C, N*Z);
        if(ct(C,tanner))
        {
            iter_success ++;
        }

        double *electron;

        awgn_channel(C, &electron, N * Z, snr_db); //模拟噪声, 01模拟成正态分布的电子数
        changeDigit(&C, electron, N * Z);

        // for(int i = 0;i<16;i++) printf("%f ",electron[i]);
        // printf("\n");
        // printDigit(C, N * Z);
        for(int i = 0;i < N * Z; i++){
            if(C_dup[i] == 0 && electron[i] > 0 || C_dup[i] == 1 && electron[i] < 0){
                 errorBit++;
            }
        }
        // 数据传输阶段 end 

        // // 译码阶段 begin
        //把错误的码传给MINBP
        int *resC;
        MINBP(C, electron, &resC, tanner, iteration, alpha, beta);
        // // 译码阶段 end

        //printDigit(resC, N*Z);
        compareDigit(resC, C_dup, N * Z);

        free(resC);
        free(C);
        free(C_dup);
        freeMatrix(S, 1);
        //freeMatrix(C2D, 1);  // free C2D
      //  printf("\n");
    }
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

    printHorizontalLine(WIDTH, '|', '=');
    
    printRow(WIDTH, " K/N    ALPHA/BETA    Bits   Frames  ITERS   BER     BER_LDPC    FER     FER_LDPC", " ");
    
    Parameters params = {
        .int1 = K*Z, .int2 = N*Z,
        .float1 = alpha, .float2 = beta,
        .int3 = allbit, .int4 = frames, .int5 = iteration,
        .float4 = simulateBER, .float5 = trueBER,
        .float6 = simulateFER, .float7 = trueFER
    };

    // 打印格式化的数字行
    printFormattedLine(params, WIDTH);
    // 打印方框的底部边界
    printHorizontalLine(WIDTH, '|', '=');
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
