#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include <stdbool.h>

// LDPC解码器函数 - 归一化最小和算法(NMS)
// 参数说明:
// H: 校验矩阵 (m x n)
// LLR_y: 接收信号的初始LLR值 (长度为n的数组)
// alpha: NMS算法的归一化因子
// iterMax: 最大迭代次数
// m: 校验节点数
// n: 变量节点数
// v: 输出解码结果 (长度为n-m的数组)
void LDPCDecoder_NMS(int** H, double* LLR_y, double alpha, int iterMax, 
                    int m, int n, int* v) {
    // 初始化变量
    double* U0i = (double*)malloc(n * sizeof(double));
    double** Uji = (double**)malloc(m * sizeof(double*));
    double** Vij = (double**)malloc(n * sizeof(double*));
    int* x = (int*)malloc(n * sizeof(int));
    
    for (int i = 0; i < m; i++) {
        Uji[i] = (double*)malloc(n * sizeof(double));
        for (int j = 0; j < n; j++) {
            Uji[i][j] = 0.0;
        }
    }
    
    for (int i = 0; i < n; i++) {
        Vij[i] = (double*)malloc(m * sizeof(double));
        U0i[i] = LLR_y[i];
        for (int j = 0; j < m; j++) {
            Vij[i][j] = 0.0;
        }
    }

    // 迭代解码
    for (int iter = 0; iter < iterMax; iter++) {
        // printf("the %d-th iteration of NMS\n", iter+1);
        
        // 1. 更新变量节点到校验节点的消息 (Vij)
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < m; j++) {
                if (H[j][i] == 1) {
                    double sum = U0i[i];
                    for (int k = 0; k < m; k++) {
                        if (H[k][i] == 1 && k != j) {
                            sum += Uji[k][i];
                        }
                    }
                    Vij[i][j] = sum;
                }
            }
        }
        
        // 2. 更新校验节点到变量节点的消息 (Uji) - 使用NMS算法
        for (int j = 0; j < m; j++) {
            for (int i = 0; i < n; i++) {
                if (H[j][i] == 1) {
                    // 计算符号乘积 (排除当前i)
                    double prodSign = 1.0;
                    // 计算最小值 (排除当前i)
                    double minVal = DBL_MAX;
                    
                    for (int k = 0; k < n; k++) {
                        if (H[j][k] == 1 && k != i) {
                            prodSign *= (Vij[k][j] >= 0) ? 1.0 : -1.0;
                            if (fabs(Vij[k][j]) < minVal) {
                                minVal = fabs(Vij[k][j]);
                            }
                        }
                    }
                    
                    Uji[j][i] = prodSign * minVal * alpha;
                }
            }
        }
        
        // 3. 硬判决
        for (int i = 0; i < n; i++) {
            double sum = U0i[i];
            for (int j = 0; j < m; j++) {
                if (H[j][i] == 1) {
                    sum += Uji[j][i];
                }
            }
            x[i] = (sum < 0) ? 1 : 0;
        }
        
        // 4. 校验是否满足所有校验方程
        bool valid = true;
        for (int j = 0; j < m; j++) {
            int sum = 0;
            for (int i = 0; i < n; i++) {
                if (H[j][i] == 1) {
                    sum += x[i];
                }
            }
            if (sum % 2 != 0) {
                valid = false;
                break;
            }
        }
        
        if (valid) {
            break; // 校验成功，提前终止迭代
        }
    }
    
    // 提取信息位 (假设后n-m位是信息位)
    for (int i = m; i < n; i++) {
        v[i-m] = x[i];
    }
    
    // 释放内存
    free(U0i);
    for (int i = 0; i < m; i++) free(Uji[i]);
    for (int i = 0; i < n; i++) free(Vij[i]);
    free(Uji);
    free(Vij);
    free(x);
}

// 测试函数
int main() {
    // 示例参数
    int m = 4;  // 校验节点数
    int n = 8;  // 变量节点数
    double alpha = 0.8; // NMS归一化因子
    int iterMax = 10; // 最大迭代次数
    
    // 示例校验矩阵 (4x8)
    int** H = (int**)malloc(m * sizeof(int*));
    for (int i = 0; i < m; i++) {
        H[i] = (int*)malloc(n * sizeof(int));
    }
    
    // 填充H矩阵 (示例)
    int exampleH[4][8] = {
        {1, 1, 1, 1, 0, 0, 0, 0},
        {1, 0, 0, 0, 1, 1, 1, 0},
        {0, 1, 0, 0, 1, 0, 0, 1},
        {0, 0, 1, 0, 0, 1, 0, 1}
    };
    
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            H[i][j] = exampleH[i][j];
        }
    }
    
    // 示例接收LLR (实际应用中应从信道获得)
    double LLR_y[8] = {1.2, -0.5, 0.8, -1.5, 0.3, -0.9, 1.1, -0.2};
    
    // 解码输出
    int* decoded = (int*)malloc((n-m) * sizeof(int));
    
    // 调用解码器
    LDPCDecoder_NMS(H, LLR_y, alpha, iterMax, m, n, decoded);
    
    // 打印解码结果
    printf("Decoded message: ");
    for (int i = 0; i < n-m; i++) {
        printf("%d ", decoded[i]);
    }
    printf("\n");
    
    // 释放内存
    for (int i = 0; i < m; i++) free(H[i]);
    free(H);
    free(decoded);
    
    return 0;
}