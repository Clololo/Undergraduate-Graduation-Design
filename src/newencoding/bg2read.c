#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "../config.h"

// ��CSV�ļ���ȡ��ͼ
int read_base_graph(const char* filename, int bg[][bgn], int* rows, int* cols) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("Error opening file %s\n", filename);
        return -1;
    }

    char line[1024];
    *rows = 0;
    *cols = 0;
    
    while (fgets(line, sizeof(line), file)) {
        char* token = strtok(line, ",");
        int col = 0;
        
        while (token) {
            bg[*rows][col++] = atoi(token);
            token = strtok(NULL, ",\n");
        }
        
        if (col > *cols) *cols = col;
        (*rows)++;
        
        if (*rows > bgm || *cols > bgn) {
            printf("Warning: Exceeded maximum base graph size\n");
            break;
        }
    }
    
    fclose(file);
    return 0;
}

// ����ѭ����λ����
void generate_circulant(int matrix[][bgz], int Z, int shift) {
    for (int i = 0; i < Z; i++) {
        for (int j = 0; j < Z; j++) {
            matrix[i][j] = (j == (i + shift) % Z) ? 1 : 0;
        }
    }
}

// ����LDPCУ�����
void generate_ldpc_matrix(int bg[][bgn], int bg_rows, int bg_cols, 
                         int Z, int output_rows, int output_cols,
                         int H[][output_cols]) {
    // ��ʱ�洢ѭ����λ����
    int circ[bgz][bgz];
    
    // ��ʼ���������Ϊȫ��
    for (int i = 0; i < output_rows; i++) {
        for (int j = 0; j < output_cols; j++) {
            H[i][j] = 0;
        }
    }
    
    // ����׼ѭ������
    for (int i = 0; i < bg_rows && i*Z < output_rows; i++) {
        for (int j = 0; j < bg_cols && j*Z < output_cols; j++) {
            if (bg[i][j] > 0) {  // ����Ԫ��
                generate_circulant(circ, Z, bg[i][j] - 1);  // ����CSV����λֵ=ʵ��ֵ+1
                
                // ��ѭ���������H�Ķ�Ӧλ��
                for (int x = 0; x < Z && i*Z+x < output_rows; x++) {
                    for (int y = 0; y < Z && j*Z+y < output_cols; y++) {
                        H[i*Z + x][j*Z + y] = circ[x][y];
                    }
                }
            }
        }
    }
}

// �������CSV�ļ�
void save_matrix_to_csv(const char* filename, int rows, int cols, int matrix[][cols]) {
    FILE* file = fopen(filename, "w");
    if (!file) {
        printf("Error creating output file\n");
        return;
    }
    
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            fprintf(file, "%d", matrix[i][j]);
            if (j < cols - 1) fprintf(file, ",");
        }
        fprintf(file, "\n");
    }
    
    fclose(file);
    printf("Matrix saved to %s\n", filename);
}

// int main() {
//     // ��CSV��ȡ��ͼ
//     int bg[MAX_BG_ROWS][MAX_BG_COLS];
//     int bg_rows, bg_cols;
    
//     if (read_base_graph("../../output/bg2.csv", bg, &bg_rows, &bg_cols) != 0) {
//         return 1;
//     }
    
//     printf("Base graph loaded: %d x %d\n", bg_rows, bg_cols);
    
//     // ��������
//     const int Z = 4;            // ��չ����
//     const int output_rows = 64; // �����������
//     const int output_cols = 128;// �����������
    
//     // �������ߴ��Ƿ�Ϸ�
//     if (output_rows > bg_rows * Z || output_cols > bg_cols * Z) {
//         printf("Error: Output size exceeds maximum possible size\n");
//         return 1;
//     }
    
//     // ����У�����
//     int H[output_rows][output_cols];
//     generate_ldpc_matrix(bg, bg_rows, bg_cols, Z, output_rows, output_cols, H);
    
//     // ������
//     save_matrix_to_csv("../../output/normal_5G_bg2.csv", output_rows, output_cols, H);
    
//     return 0;
// }