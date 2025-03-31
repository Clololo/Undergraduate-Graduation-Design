#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "./bg2read.h"
/**
 * @brief ����QC-LDPCУ�����H�����Ӿ���Hp��Hs
 * @param H_block �����ͼ���󣨴洢��λֵ��0��ʾȫ�����
 * @param mb ��ͼ����
 * @param nb ��ͼ����
 * @param z ��չ����
 * @param H ���������У�������Ԥ�����ڴ棬�ߴ�mb*z �� nb*z��
 * @param Hp �����Hp�Ӿ�����Ԥ�����ڴ棬�ߴ�mb*z �� mb*z��
 * @param Hs �����Hs�Ӿ�����Ԥ�����ڴ棬�ߴ�mb*z �� (nb-mb)*z��
 */
void HxMatrixGen(int H_block[][bgn], int mb, int nb, int z, int** H, int** Hp, int** Hs) {
    int i, j, k, idx;
    int kb = nb - mb;
    
    // 1. ��ʼ��ȫ�����
    for (i = 0; i < mb*z; i++) {
        for (j = 0; j < nb*z; j++) {
            H[i][j] = 0;
        }
    }
    
    // 2. ����׼ѭ������H
    for (i = 0; i < mb; i++) {
        for (j = 0; j < nb; j++) {
            idx = H_block[i][j];
            if (idx == 0) continue; // ����ȫ�����
            
            // ����ѭ����λ����(z��z)
            for (k = 0; k < z; k++) {
                int shift = (k + idx - 1) % z; // MATLAB������1��ʼ��C��0��ʼ
                H[i*z + k][j*z + shift] = 1;
            }
        }
    }
    
    // 3. ������������ӦMATLAB��H(1,1008)=0��
    H[0][63] = 0; // C������0��ʼ
    
    // 4. ��ȡHp��Hs�Ӿ���
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
 * @brief LDPC�����㷨2
 * @param Hs ����Hs���󣨳ߴ�mb*z �� kb*z��
 * @param Hp ����Hp���󣨳ߴ�mb*z �� mb*z��
 * @param s ������Ϣ���У�����kb*z��
 * @param mb ��ͼ����
 * @param kb ��ͼ���� - ����
 * @param z ��չ����
 * @param x ����������У���Ԥ�����ڴ棬����(mb+kb)*z��
 */
void Encoder2(int** Hs, int** Hp, int* s, int mb, int kb, int z, int* x) {
    int i, j, idx;
    int total_len = (mb + kb) * z;
    int* w = (int*)calloc(mb * z, sizeof(int)); // �м���
    int* p = (int*)calloc(mb * z, sizeof(int)); // У�����
    
    // 1. ����w = s * Hs^T
    for (i = 0; i < mb*z; i++) {
        for (j = 0; j < kb*z; j++) {
            w[i] ^= (s[j] & Hs[i][j]);    // ��λ���ʵ��ģ2�ӷ� // ����˷�
           // printf("w[%d] = %d\n",i,w[i]);
        } 
    }

    // 2. ����У�����p���㷨2��
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
    p[mb*z-1] = (w[mb*z-1] + p[(mb-1)*z]) % 2; // ���һλ
    
    // 3. ��ϱ�������x = [p s]
    memcpy(x, p, mb*z * sizeof(int));
    memcpy(x + mb*z, s, kb*z * sizeof(int));
    
    free(w);
    free(p);
}


// // ʾ��ʹ��
// int main() {
//     // ��������
//     int mb = 18, nb = 36, z = 4;
//     int kb = nb - mb;
    
//     // 1. ׼����ͼ��ʾ�����ݣ�ʵ��Ӧ���ļ���ȡ��
//     int **H_block;


//     // 2. �����ڴ�
//     int** H = (int**)malloc(mb*z * sizeof(int*));
//     int** Hp = (int**)malloc(mb*z * sizeof(int*));
//     int** Hs = (int**)malloc(mb*z * sizeof(int*));
//     for (int i = 0; i < mb*z; i++) {
//         H[i] = (int*)malloc(nb*z * sizeof(int));
//         Hp[i] = (int*)malloc(mb*z * sizeof(int));
//         Hs[i] = (int*)malloc(kb*z * sizeof(int));
//     }
    
//     // 3. ���ɾ���
//     HxMatrixGen(H_block, mb, nb, z, H, Hp, Hs);
    
//     // 4. ����ʾ��
//     int* s = (int*)calloc(kb*z, sizeof(int));
//     int* x = (int*)malloc((mb+kb)*z * sizeof(int));
//     Encoder2(Hs, Hp, s, mb, kb, z, x);
    
//     // 5. �ͷ��ڴ�
//     // ... (ʡ���ͷŴ���)
//     free(H);
//     free(Hp);
//     free(Hs);

//     return 0;
// }