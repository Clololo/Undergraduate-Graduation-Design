#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "../config.h"
#include "./bg2read.h"
#include "../tools/utils.h"
#include "../tools/mat_tool.h"

/**
 * @brief 通过基矩阵生成QC-LDPC校验矩阵H及其子矩阵Hp、Hs
 * @param H_block 输入基图矩阵（存储移位值，0表示全零矩阵）
 * @param mb 基图行数
 * @param nb 基图列数
 * @param z 扩展因子
 * @param H 输出的完整校验矩阵（需预分配内存，尺寸mb*z × nb*z）
 * @param Hp 输出的Hp子矩阵（需预分配内存，尺寸mb*z × mb*z）
 * @param Hs 输出的Hs子矩阵（需预分配内存，尺寸mb*z × (nb-mb)*z）
 */
void HGenerator(int H_block[][bgn], int mb, int nb, int z, int** H, int** Hp, int** Hs) {
    int i, j, k, idx;
    int kb = nb - mb;
    
    for (i = 0; i < mb*z; i++) {
        for (j = 0; j < nb*z; j++) {
            H[i][j] = 0;
        }
    }
    
    for (i = 0; i < mb; i++) {
        for (j = 0; j < nb; j++) {
            idx = H_block[i][j] % z;
            if (idx == 0) continue; // 跳过全零矩阵
            
            for (k = 0; k < z; k++) {
                int shift = (k + idx - 1) % z; 
                H[i*z + k][j*z + shift] = 1;
            }
        }
    }
    
    for (i = 0; i < mb*z; i++) {
        for (j = 0; j < mb*z; j++) {
            Hp[i][j] = H[i][j];
        }
        for (j = mb*z; j < nb*z; j++) {
            Hs[i][j-mb*z] = H[i][j];
        }
    }
}

/**
 * @brief 读取校验矩阵H及其子矩阵Hp、Hs
 * @param filename 校验矩阵文件
 * @param m 行数
 * @param n 列数
 * @param H 输出的完整校验矩阵（需预分配内存，尺寸mb*z × nb*z）
 * @param Hp 输出的Hp子矩阵（需预分配内存，尺寸mb*z × mb*z）
 * @param Hs 输出的Hs子矩阵（需预分配内存，尺寸mb*z × (nb-mb)*z）
 */

void PSOHGenerator(const char *filename, int m, int n, int** H, int** Hp, int** Hs) {
    int i, j;
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error opening file %s\n", filename);
        return;
    }

    for (i = 0; i < m; i++) {
        for (j = 0; j < n; j++) {
            if (fscanf(file, "%d,", &H[i][j]) != 1) {
                printf("Error reading data at [%d][%d]\n", i, j);
                fclose(file);
                return;
            }
        }
        fgetc(file);
    }

    for (i = 0; i < m; i++) {
        for (j = 0; j < m; j++) {
            Hp[i][j] = H[i][j];
        }
        for (j = m; j < n; j++) {
            Hs[i][j - m] = H[i][j];
        }
    }
    printf("get a %d*%d H from file %s\n", m, n, filename);

    fclose(file);
}

/**
 * @brief LDPC编码算法2
 * @param Hs 输入Hs矩阵（尺寸mb*z × kb*z）
 * @param Hp 输入Hp矩阵（尺寸mb*z × mb*z）
 * @param s 输入信息序列（长度kb*z）
 * @param mb 基图行数
 * @param kb 基图列数 - 行数
 * @param z 扩展因子
 * @param x 输出编码序列（需预分配内存，长度(mb+kb)*z）
 */
void Encoder(int** H, int** Hs, int** Hp, int* s, int mb, int kb, int z, int* x) {
    int i, j, idx;
    int total_len = (mb + kb) * z;
    int* w = (int*)calloc(mb * z, sizeof(int)); // 中间结果
    int* p = (int*)calloc(mb * z, sizeof(int)); // 校验比特
    
    // 1. 计算w = s * Hs^T
    for (i = 0; i < mb*z; i++) {
        for (j = 0; j < kb*z; j++) {
            w[i] ^= (s[j] & Hs[i][j]);    
            // printf("w[%d] = %d\n",i,w[i]);
        } 
    }

    int** HpT = (int**)malloc((mb+kb)*z * sizeof(int*));
    int** HpT_inv = (int**)malloc((mb+kb)*z * sizeof(int*));
    for (int i = 0; i < (mb+kb)*z; i++) {
        HpT[i] = (int*)malloc((mb+kb)*z * sizeof(int));
        HpT_inv[i] = (int*)malloc((mb+kb)*z * sizeof(int));
    }

    transposeMatrix(Hp, HpT, mb*z, mb*z);
    matrix_inverse(HpT, HpT_inv, mb*z);

    // 2. 计算p = w * (Hp^T)^(-1)
    for (i = 0; i < mb*z; i++) {
        for (j = 0; j < mb*z; j++) {
            p[i] ^= (w[j] & HpT_inv[j][i]);   
        } 
    }

    // // 2. 计算校验比特p（算法2）
    // p[0] = w[0] % 2;
    // idx = 0;
    // for (i = 0; i < mb*z; i++) {
    //     if (idx > (mb-1)*z && idx <= mb*z-1) {
    //         int new_idx = idx - (mb-1)*z;
    //         p[new_idx] ^= (w[new_idx] & p[idx]);
    //         idx = new_idx;
    //     } 
    //     else if (idx >= 0 && idx <= (mb-1)*z) {
    //         p[z + idx] ^= (w[z + idx] & p[idx]);
    //         idx += z;
    //     }
    // }
    // p[mb*z-1] ^= (w[mb*z-1] & p[(mb-1)*z]); // 最后一位

    // 3. 组合编码序列x = [p s]
    memcpy(x, p, mb*z * sizeof(int));
    memcpy(x + mb*z, s, kb*z * sizeof(int));
    
    //printf("VALID = %d\n",check_codeword(H, x, mb*z, (mb+kb)*z));
    free(w);
    free(p);
}