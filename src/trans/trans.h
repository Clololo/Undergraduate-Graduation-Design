#ifndef TRANS_H
#define TRANS_H

// BPSK调制函数
void BPSK_Modulation(int* x, double* d, int len);

// AWGN信道函数
void AWGN_Channel(double* d, double* y, int len, double SNR_dB, double R);

// 译码端接收函数
void Receiver_LLR(double* y, double* LLR_y, int len, double sigma);

#endif