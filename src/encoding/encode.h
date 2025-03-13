/*
 该部分用于
 创建校验矩阵QC-LDPC、
 根据校验矩阵得到生成矩阵、
 根据校验矩阵创建 tanner 图
*/

#ifndef ENCODE_H
#define ENCODE_H
#include <stdbool.h>

// 邻接表的节点结构
typedef struct AdjListNode {
    int dest;
    struct AdjListNode* next;
} AdjListNode;

// 邻接表
typedef struct AdjList {
    AdjListNode* head;
} AdjList;

// 图结构
typedef struct Graph {
    int numNodes;
    int numC;       // 校验节点数
    int numV;    // 变量节点数
    AdjList* array;
} Graph;

void generateQCLDPCMatrix(int M, int N, int Z, int ****H);
void printQCLDPCMatrix(int M, int N, int Z, int ***H);
void freeQCLDPCMatrix(int M, int N, int Z, int ***H);
void convertTo2DMatrix(int M, int N, int Z, int ***H, int ***H2D);

//void copyMatrix(int **H, int **H2D, int rows, int cols);
void gaussianElimination(int M, int N, int **H);
void getGeneratorMatrix(int M, int N, int **H, int ***G);

AdjListNode* newAdjListNode(int dest);
Graph* createGraph(int numNodes, int M, int N);
void addEdge(Graph* graph, int src, int dest);
void printGraph(Graph* graph);
Graph* constructTannerGraph(int M, int N, int **H);
void freeGraph(Graph* graph);

bool MatrixDivide(int ***H, int ***A, int ***B, int ***T, int ***C, int ***D, int ***E, int m, int n, int g);
void calculateResult(int **C, int **E, int **T, int **A, int ***result, int m, int n, int p, int r);
#endif
