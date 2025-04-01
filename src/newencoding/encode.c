#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "../config.h"
#include "./bg2read.h"
#include "../tools/utils.h"
#include "../tools/mat_tool.h"

/**
 * @brief ͨ������������QC-LDPCУ�����H�����Ӿ���Hp��Hs
 * @param H_block �����ͼ���󣨴洢��λֵ��0��ʾȫ�����
 * @param mb ��ͼ����
 * @param nb ��ͼ����
 * @param z ��չ����
 * @param H ���������У�������Ԥ�����ڴ棬�ߴ�mb*z �� nb*z��
 * @param Hp �����Hp�Ӿ�����Ԥ�����ڴ棬�ߴ�mb*z �� mb*z��
 * @param Hs �����Hs�Ӿ�����Ԥ�����ڴ棬�ߴ�mb*z �� (nb-mb)*z��
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
            if (idx == 0) continue; // ����ȫ�����
            
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
 * @brief ��ȡУ�����H�����Ӿ���Hp��Hs
 * @param filename У������ļ�
 * @param m ����
 * @param n ����
 * @param H ���������У�������Ԥ�����ڴ棬�ߴ�mb*z �� nb*z��
 * @param Hp �����Hp�Ӿ�����Ԥ�����ڴ棬�ߴ�mb*z �� mb*z��
 * @param Hs �����Hs�Ӿ�����Ԥ�����ڴ棬�ߴ�mb*z �� (nb-mb)*z��
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
 * @brief LDPC�����㷨2
 * @param Hs ����Hs���󣨳ߴ�mb*z �� kb*z��
 * @param Hp ����Hp���󣨳ߴ�mb*z �� mb*z��
 * @param s ������Ϣ���У�����kb*z��
 * @param mb ��ͼ����
 * @param kb ��ͼ���� - ����
 * @param z ��չ����
 * @param x ����������У���Ԥ�����ڴ棬����(mb+kb)*z��
 */
void Encoder(int** H, int** Hs, int** Hp, int* s, int mb, int kb, int z, int* x) {
    int i, j, idx;
    int total_len = (mb + kb) * z;
    int* w = (int*)calloc(mb * z, sizeof(int)); // �м���
    int* p = (int*)calloc(mb * z, sizeof(int)); // У�����
    
    // 1. ����w = s * Hs^T
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

    // 2. ����p = w * (Hp^T)^(-1)
    for (i = 0; i < mb*z; i++) {
        for (j = 0; j < mb*z; j++) {
            p[i] ^= (w[j] & HpT_inv[j][i]);   
        } 
    }

    // // 2. ����У�����p���㷨2��
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
    // p[mb*z-1] ^= (w[mb*z-1] & p[(mb-1)*z]); // ���һλ

    // 3. ��ϱ�������x = [p s]
    memcpy(x, p, mb*z * sizeof(int));
    memcpy(x + mb*z, s, kb*z * sizeof(int));
    
    //printf("VALID = %d\n",check_codeword(H, x, mb*z, (mb+kb)*z));
    free(w);
    free(p);
}