#ifndef TRANS_H
#define TRANS_H

// BPSK���ƺ���
void BPSK_Modulation(int* x, double* d, int len);

// AWGN�ŵ�����
void AWGN_Channel(double* d, double* y, int len, double SNR_dB, double R);

// ����˽��պ���
void Receiver_LLR(double* y, double* LLR_y, int len, double sigma);

#endif