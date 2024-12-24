#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "encode.h"
#include "../tools/mat_tool.h"

//To generate the initial H matrix
void generateQCLDPCMatrix(int M, int N, int Z, int ****H) {
    // Allocate memory for H matrix
    *H = (int ***)malloc(M * sizeof(int **));
    for (int i = 0; i < M; i++) {
        (*H)[i] = (int **)malloc(N * sizeof(int *));
        for (int j = 0; j < N; j++) {
            (*H)[i][j] = (int *)calloc(Z * Z, sizeof(int));
        }
    }

    // Seed the random number generator
    srand(time(NULL));

    // Generate the QC-LDPC matrix
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < N; j++) {
            // Decide if this submatrix will be a zero matrix or a circulant matrix
            if (rand() % Z) {
                // Generate a circulant matrix
                if(rand() % 10 < 3) continue;  //让矩阵更稀疏
                int shift = rand() % Z;
                for (int k = 0; k < Z; k++) {
                    (*H)[i][j][k * Z + (k + shift) % Z] = 1;
                }
            } // else a zero matrix
        }
    }
}


void convertTo2DMatrix(int M, int N, int Z, int ***H, int ***H2D) {
    // Allocate memory for the new 2D matrix
    *H2D = (int **)malloc(M * Z * sizeof(int *));
    for (int i = 0; i < M * Z; i++) {
        (*H2D)[i] = (int *)malloc(N * Z * sizeof(int));
    }

    // Convert the 3D QC-LDPC matrix to 2D
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < N; j++) {
            for (int row = 0; row < Z; row++) {
                for (int col = 0; col < Z; col++) {
                    (*H2D)[i * Z + row][j * Z + col] = H[i][j][row * Z + col];
                }
            }
        }
    }
}

// void copyMatrix(int **H, int **H2D, int rows, int cols) {
//     int **H = (int **)malloc(rows * sizeof(int *));
//     for (int i = 0; i < rows; i++) {
//         H[i] = (int *)malloc(cols * sizeof(int));
//     }

//     for (int i = 0; i < rows; i++) {
//         for (int j = 0; j < cols; j++) {
//             H[i][j] = H2D[i][j];
//         }
//     }
// }

//To get GeneratorMatrix
void gaussianElimination(int M, int N, int **H) {
    int i, j, k;
    for (i = 0; i < M; i++) {
        // Find the pivot and normalize the row
        int pivot = -1;
        for (j = i; j < N; j++) {
            if (H[i][j] == 1) {
                pivot = j;
                break;
            }
        }
        if (pivot == -1) {
            continue; // This row is all zeros, skip it
        }

        // Eliminate all other entries in the pivot column
        for (j = 0; j < M; j++) {
            if (j != i && H[j][pivot] == 1) {
                for (k = pivot; k < N; k++) {
                    H[j][k] ^= H[i][k];
                }
            }
        }
    }

    // Rearrange the matrix so that the left side is the (M x (N-M)) matrix and the right side is the (M x M) identity matrix
    for (i = 0; i < M; i++) {
        for (j = 0; j < M; j++) {
            if (i == j) {
                H[i][N - M + j] = 1;
            } else {
                H[i][N - M + j] = 0;
            }
        }
    }
}

void getGeneratorMatrix(int M, int N, int **H, int ***G) {
    int i, j;

    // Allocate memory for G
    *G = (int **)malloc((N - M) * sizeof(int *));
    for (i = 0; i < (N - M); i++) {
        (*G)[i] = (int *)malloc(N * sizeof(int));
    }

    // Perform Gaussian elimination to transform H into standard form
    gaussianElimination(M, N, H);
    // printf("\nafter gauss:\n");
    // for(int i = 0; i < M; i++){
    //     for(int j = 0; j < N; j++){
    //         printf("%d ",H[i][j]);
    //     }
    //     printf("\n");
    // }

    // Fill in the left part of G with the identity matrix
    for (i = 0; i < (N - M); i++) {
        for (j = 0; j < (N - M); j++) {
            (*G)[i][j] = (i == j) ? 1 : 0;
        }
    }

    // Fill in the right part of G with the transpose of the left part of H
    for (i = 0; i < (N - M); i++) {
        for (j = 0; j < M; j++) {
            (*G)[i][j + (N - M)] = H[j][i];
        }
    }
}

// 计算 C + E * T * A
void calculateResult(int **C, int **E, int **T, int **A, int ***result, int m, int n, int p, int r) {
    int **ET, **ETA;

    // 计算 E * T
    matrixMultiply(E, T, &ET, m, n, p);

    // 计算 (E * T) * A
    matrixMultiply(ET, A, &ETA, m, p, r);

    // 计算 C + ETA
    matrixAdd(C, ETA, result, m, r);

    // 释放临时矩阵的内存
    for (int i = 0; i < m; i++) {
        free(ET[i]);
        free(ETA[i]);
    }
    free(ET);
    free(ETA);
}

// 高斯消元，将矩阵变换为下三角形式
bool GenerateLowTriMat(int ***matrix, int m, int n,int g) {
    int pivot_row = 0;
    int pivot_col = n - m + g;

    // printf("Matrix h:\n");
    // printMatrix(*matrix, (m), (n));

    while (pivot_row < m - g && pivot_col < n) {
        //printf("%d %d %d\n",pivot_row,pivot_col,(*matrix)[pivot_row][pivot_col]);
        if(((*matrix)[pivot_row][pivot_col])==0)
        {
            int max_row = -1;
            for (int i = pivot_row + 1; i < m; i++) {
                if ((*matrix)[i][pivot_col]) {
                    max_row = i;
                    break;
                }
            }
            if (max_row != -1) {
               // printf("swap %d %d\n",max_row, pivot_row);
                //swap_rows((*matrix), max_row, pivot_row, n);
                for (int i = 0; i < n; i++) {
                    int temp = (*matrix)[max_row][i];
                    (*matrix)[max_row][i] = (*matrix)[pivot_row][i];
                    (*matrix)[pivot_row][i] = temp;
                }
                //printf("swap %d %d Matrix h:\n",max_row, pivot_row);
                // printMatrix(*matrix, (m), (n));
            }
            else{
                return false;
            }
        }

        pivot_row++;
        pivot_col++;
    }
    // printf("--------------------------------------------------------\n");
    // printMatrix(*matrix, (m), (n));
    //     printf("--------------------------------------------------------\n");
    int now_row = m - g - 1;
    while(now_row){
        for(int row = now_row-1; row>=0; row--){
            if((*matrix)[row][now_row + n - m + g]){
                //printf("%d row sub %d\n",row, now_row);
                for(int i = 0;i < n;i++){
                    (*matrix)[row][i] ^= (*matrix)[now_row][i];
                }
            }
        }
        now_row--;
    }
    for(int i = 0;i < m-g;i++){
        int j = i+ n - m + g;
        if((*matrix)[i][j]==0) return false;
    }
    return true;
}



// 将矩阵划分为六个部分，并进行高斯消元
bool MatrixDivide(int ***H, int ***A, int ***B, int ***T, int ***C, int ***D, int ***E, int m, int n, int g) {
    // 分配内存给六个部分矩阵
    *A = (int **)malloc((m - g) * sizeof(int *));
    *B = (int **)malloc((m - g) * sizeof(int *));
    *T = (int **)malloc((m - g) * sizeof(int *));
    *C = (int **)malloc(g * sizeof(int *));
    *D = (int **)malloc(g * sizeof(int *));
    *E = (int **)malloc(g * sizeof(int *));
    
    for (int i = 0; i < (m - g); i++) {
        (*A)[i] = (int *)malloc((n - m) * sizeof(int));
        (*B)[i] = (int *)malloc(g * sizeof(int));
        (*T)[i] = (int *)malloc((m - g) * sizeof(int));
    }
    for (int i = 0; i < g; i++) {
        (*C)[i] = (int *)malloc((n - m) * sizeof(int));
        (*D)[i] = (int *)malloc(g * sizeof(int));
        (*E)[i] = (int *)malloc((m - g) * sizeof(int));
    }
    

    // 对整个 H 矩阵进行高斯消元，使右上角的 (m-g) x (m-g) 部分变为下三角形式
    bool flag = GenerateLowTriMat(H, m, n, g);


    if(!flag){
        // printf("Matrix !h:\n");
        // printMatrix(*H, (m), (n));

        return false;
    }


        // 复制矩阵 H 到 T 矩阵中
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            if (i < (m - g) && j < (n - m)) {
                (*A)[i][j] = (*H)[i][j];
            } else if (i < (m - g) && j >= (n - m) && j < (n - m + g)) {
                (*B)[i][j - (n - m)] = (*H)[i][j];
            } else if (i < (m - g) && j >= (n - m + g) && j < n) {
                (*T)[i][j - (n - m + g)] = (*H)[i][j];
            } else if (i >= (m - g) && i < m && j < (n - m)) {
                (*C)[i - (m - g)][j] = (*H)[i][j];
            } else if (i >= (m - g) && i < m && j >= (n - m) && j < (n - m + g)) {
                (*D)[i - (m - g)][j - (n - m)] = (*H)[i][j];
            } else if (i >= (m - g) && i < m && j >= (n - m + g) && j < n) {
                (*E)[i - (m - g)][j - (n - m + g)] = (*H)[i][j];
            }
        }
    }
    // printf("Matrix h:\n");
    // printMatrix(*H, (m), (n));

    // printf("Matrix h:\n");
    // printMatrix(*H, (m), (n));
    // // 打印结果（示例）
    // printf("Matrix A:\n");
    // printMatrix(*A, (m - g), (n - m));
    // printf("\nMatrix B:\n");
    // printMatrix(*B, (m - g), g);
    // printf("\nMatrix T (upper triangular):\n");
    // printMatrix(*T, (m - g), (m - g));  // 打印近似下三角形式的 T 矩阵
    // printf("\nMatrix C:\n");
    // printMatrix(*C, g, (n - m));
    // printf("\nMatrix D:\n");
    // printMatrix(*D, g, g);
    // printf("\nMatrix E:\n");
    // printMatrix(*E, g, (m - g));
    return true;
}


//TO print

void printQCLDPCMatrix(int M, int N, int Z, int ***H) {
    for (int i = 0; i < M; i++) {
        for (int row = 0; row < Z; row++) {
            for (int j = 0; j < N; j++) {
                for (int col = 0; col < Z; col++) {
                    printf("%d", H[i][j][row * Z + col]);
                }
                printf("");
            }
            printf("\n");
        }
        printf("\n");
    }
}


//To free

void freeQCLDPCMatrix(int M, int N, int Z, int ***H) {
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < N; j++) {
            free(H[i][j]);
        }
        free(H[i]);
    }
    free(H);
}


//generate tanner graph
// 创建新的邻接表节点
AdjListNode* newAdjListNode(int dest) {
    AdjListNode* newNode = (AdjListNode*)malloc(sizeof(AdjListNode));
    newNode->dest = dest;
    newNode->next = NULL;
    return newNode;
}

// 创建图
Graph* createGraph(int numNodes, int M, int N) {
    Graph* graph = (Graph*)malloc(sizeof(Graph));
    graph->numNodes = numNodes;
    graph->numC = M;
    graph->numV = N;
    graph->array = (AdjList*)malloc(numNodes * sizeof(AdjList));
    for (int i = 0; i < numNodes; ++i) {
        graph->array[i].head = NULL;
    }
    return graph;
}

// 添加边
void addEdge(Graph* graph, int src, int dest) {
    AdjListNode* newNode = newAdjListNode(dest);
    newNode->next = graph->array[src].head;
    graph->array[src].head = newNode;

    // tanner图是无向图，需要双向添加
    newNode = newAdjListNode(src);
    newNode->next = graph->array[dest].head;
    graph->array[dest].head = newNode;
}

// 打印图
void printGraph(Graph* graph) {
    for (int i = 0; i < graph->numNodes; ++i) {
        AdjListNode* pCrawl = graph->array[i].head;
        printf("Adjacency list of node %d\n head", i);
        while (pCrawl) {
            printf(" -> %d", pCrawl->dest);
            pCrawl = pCrawl->next;
        }
        printf("\n");
    }
}

// 根据校验矩阵构建Tanner图
Graph* constructTannerGraph(int M, int N, int **H) {
    int numNodes = M + N;   //M个校验节点，N个变量节点
    Graph* graph = createGraph(numNodes, M, N);

    for (int i = 0; i < M; i++) {
        for (int j = 0; j < N; j++) {
            if (H[i][j] == 1) {
                addEdge(graph, i, M + j);
            }
        }
    }

    return graph;
}

// 释放图的内存
void freeGraph(Graph* graph){

    for (int i = 0; i < graph->numNodes; ++i) {
        AdjListNode* pCrawl = graph->array[i].head;
        while (pCrawl) {
            AdjListNode* temp = pCrawl;
            pCrawl = pCrawl->next;
            free(temp);
        }
    }
    free(graph->array);
    free(graph);
    return;
}