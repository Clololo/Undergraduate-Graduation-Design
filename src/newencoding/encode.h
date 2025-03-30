#ifndef ENCODE2_H
#define ENCODE2_H

#include "../config.h"

void HxMatrixGen(int H_block[][bgn], int mb, int nb, int z, int** H, int** Hp, int** Hs);
void Encoder2(int** Hs, int** Hp, int* s, int mb, int kb, int z, int* x);

#endif
