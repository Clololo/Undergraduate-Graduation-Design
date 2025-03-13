/*
 �ò�������
 ����У�����QC-LDPC��
 ����У�����õ����ɾ���
 ����У����󴴽� tanner ͼ
*/

#ifndef ENCODE_H
#define ENCODE_H
#include <stdbool.h>

// �ڽӱ�Ľڵ�ṹ
typedef struct AdjListNode {
    int dest;
    struct AdjListNode* next;
} AdjListNode;

// �ڽӱ�
typedef struct AdjList {
    AdjListNode* head;
} AdjList;

// ͼ�ṹ
typedef struct Graph {
    int numNodes;
    int numC;       // У��ڵ���
    int numV;    // �����ڵ���
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
