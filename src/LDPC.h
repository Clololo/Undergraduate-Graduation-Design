#ifndef LDPC_H
#define LDPC_H

#include <stdbool.h>

void run(int frames, double Eb_N0_dB, int iteration, float alpha, float beta, bool UsePSOGenH);

#endif