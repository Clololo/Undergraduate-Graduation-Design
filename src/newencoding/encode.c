#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "./bg2read.h"
/**
 * @brief 生成QC-LDPC校验矩阵H及其子矩阵Hp、Hs
 * @param H_block 输入基图矩阵（存储移位值，0表示全零矩阵）
 * @param mb 基图行数
 * @param nb 基图列数
 * @param z 扩展因子
 * @param H 输出的完整校验矩阵（需预分配内存，尺寸mb*z × nb*z）
 * @param Hp 输出的Hp子矩阵（需预分配内存，尺寸mb*z × mb*z）
 * @param Hs 输出的Hs子矩阵（需预分配内存，尺寸mb*z × (nb-mb)*z）
 */
void HxMatrixGen(int H_block[][bgn], int mb, int nb, int z, int** H, int** Hp, int** Hs) {
    int i, j, k, idx;
    int kb = nb - mb;
    
    // 1. 初始化全零矩阵
    for (i = 0; i < mb*z; i++) {
        for (j = 0; j < nb*z; j++) {
            H[i][j] = 0;
        }
    }
    
    // 2. 生成准循环矩阵H
    for (i = 0; i < mb; i++) {
        for (j = 0; j < nb; j++) {
            idx = H_block[i][j];
            if (idx == 0) continue; // 跳过全零矩阵
            
            // 生成循环移位矩阵(z×z)
            for (k = 0; k < z; k++) {
                int shift = (k + idx - 1) % z; // MATLAB索引从1开始，C从0开始
                H[i*z + k][j*z + shift] = 1;
            }
        }
    }
    
    // 3. 特殊修正（对应MATLAB的H(1,1008)=0）
    H[0][63] = 0; // C索引从0开始
    
    // 4. 提取Hp和Hs子矩阵
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
 * @brief LDPC编码算法2
 * @param Hs 输入Hs矩阵（尺寸mb*z × kb*z）
 * @param Hp 输入Hp矩阵（尺寸mb*z × mb*z）
 * @param s 输入信息序列（长度kb*z）
 * @param mb 基图行数
 * @param kb 基图列数 - 行数
 * @param z 扩展因子
 * @param x 输出编码序列（需预分配内存，长度(mb+kb)*z）
 */
void Encoder2(int** Hs, int** Hp, int* s, int mb, int kb, int z, int* x) {
    int i, j, idx;
    int total_len = (mb + kb) * z;
    int* w = (int*)calloc(mb * z, sizeof(int)); // 中间结果
    int* p = (int*)calloc(mb * z, sizeof(int)); // 校验比特
    
    // 1. 计算w = s * Hs^T
    for (i = 0; i < mb*z; i++) {
        for (j = 0; j < kb*z; j++) {
            w[i] ^= (s[j] & Hs[i][j]);    // 按位异或实现模2加法 // 矩阵乘法
           // printf("w[%d] = %d\n",i,w[i]);
        } 
    }

    // 2. 计算校验比特p（算法2）
    p[0] = w[0] % 2;
    idx = 0;
    for (i = 0; i < mb*z; i++) {
        if (idx > (mb-1)*z && idx <= mb*z-1) {
            int new_idx = idx - (mb-1)*z;
            p[new_idx] = (w[new_idx] + p[idx]) % 2;
            idx = new_idx;
        } 
        else if (idx >= 0 && idx <= (mb-1)*z) {
            p[z + idx] = (w[z + idx] + p[idx]) % 2;
            idx += z;
        }
    }
    p[mb*z-1] = (w[mb*z-1] + p[(mb-1)*z]) % 2; // 最后一位
    
    // 3. 组合编码序列x = [p s]
    memcpy(x, p, mb*z * sizeof(int));
    memcpy(x + mb*z, s, kb*z * sizeof(int));
    
    free(w);
    free(p);
}


// // 示例使用
// int main() {
//     // 参数配置
//     int mb = 18, nb = 36, z = 4;
//     int kb = nb - mb;
    
//     // 1. 准备基图（示例数据，实际应从文件读取）
//     int **H_block;


//     // 2. 分配内存
//     int** H = (int**)malloc(mb*z * sizeof(int*));
//     int** Hp = (int**)malloc(mb*z * sizeof(int*));
//     int** Hs = (int**)malloc(mb*z * sizeof(int*));
//     for (int i = 0; i < mb*z; i++) {
//         H[i] = (int*)malloc(nb*z * sizeof(int));
//         Hp[i] = (int*)malloc(mb*z * sizeof(int));
//         Hs[i] = (int*)malloc(kb*z * sizeof(int));
//     }
    
//     // 3. 生成矩阵
//     HxMatrixGen(H_block, mb, nb, z, H, Hp, Hs);
    
//     // 4. 编码示例
//     int* s = (int*)calloc(kb*z, sizeof(int));
//     int* x = (int*)malloc((mb+kb)*z * sizeof(int));
//     Encoder2(Hs, Hp, s, mb, kb, z, x);
    
//     // 5. 释放内存
//     // ... (省略释放代码)
//     free(H);
//     free(Hp);
//     free(Hs);

//     return 0;
// }