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


int errorBit = 0;  //��¼����ʵ�ʷ�����λ��
int errorBitWithLDPC = 0;  //��¼�����޷�������λ��
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
        if (sum) return 0;  // ����δͨ��
    }
    return 1;  // ����ͨ��
}


int main(){
    errorBit = 0;  //��¼����ʵ�ʷ�����λ��
    errorBitWithLDPC = 0;  //��¼�����޷�������λ��
    errorFrame = 0;
    errorFrameWithLDPC = 0;
    iter_success = 0;
    //�������� begin
    int iter_num = 20;
    int M = 4;  //  У��ڵ����
    int N = 8;  // ����������λ �������ڵ������
    int Z = 4;  // Size of each submatrix (Z x Z)
    int g = 4;  // g ��Ϊ���Ʊ�ʾ�� gap��g ԽС�����븴�Ӷ�Խ��
    int K = N - M; // ��Ϣλ
    int Slen = (N - M) * Z;  // ��������Ϣλ
    long long allbit = (20 * N * Z);

    srand(time(NULL) + 1);  //Ϊ���������봴�����������
    //�������� end

    int ***H;
    int **H2D,**A,**B,**T,**C,**D,**E,**CC,**DD;
    Graph* tanner;

    //����׶� begin
    while(1)
    {
        generateQCLDPCMatrix(M, N, Z, &H);
        convertTo2DMatrix(M, N, Z, H, &H2D);
       // printMatrix(H2D, M * Z, N * Z);

        tanner = constructTannerGraph(M * Z, N * Z, H2D);
        if(MatrixDivide(&H2D, &A, &B, &T, &C, &D, &E, M*Z, N*Z, g)){
            break;
        }
     freeQCLDPCMatrix(M, N, Z, H);   //free H
    freeMatrix(H2D, M * Z);  // free H2D
    //freeMatrix(G, M * Z);  //free G
    freeGraph(tanner);  //free tanner
    }
    printQCLDPCMatrix(M, N, Z, H);
    printf("Generated 2D QC-LDPC H matrix:\n");
    printMatrix(H2D, M * Z, N * Z);
    printGraph(tanner);   //�ڸ�˹��Ԫ֮ǰ�õ�tannerͼ


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
    //����ʹ�����ɾ���ķ�ʽ
    int **G;
    
    getGeneratorMatrix(M * Z, N * Z, H2D, &G);
    printf("\n");
    printf("Generated 2D QC-LDPC G matrix:\n");
    printMatrix(G, (N - M) * Z, N * Z);
    */
    
    // ����׶� end


    for(int i = 1;i<=20;i++)
    {
        //printf("----------------------------\n");
        // ���ݴ���׶� begin
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

        //matrixMultiply(S, G, &C2D, 1, Slen, N*Z);    //���ɾ��� ����˵õ�����C

        int *C;   
        int *C_dup;
        fillArrayWithFirstRow(S, p1, p2, &C, (N-M)*Z, g, M * Z - g);
        fillArrayWithFirstRow(S, p1, p2, &C_dup, (N-M)*Z, g, M * Z - g);
        // generateDigit(C2D, N * Z, &C);
        // generateDigit(C2D, N * Z, &C_dup);   //���ɾ��� �õ�����
        //printDigit(C, N*Z);
        if(ct(C,tanner))
        {
            iter_success ++;
        }

    }
    printf("%d %d\n",iter_success, 20);
}
