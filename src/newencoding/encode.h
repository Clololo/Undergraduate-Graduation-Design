#ifndef ENCODE2_H
#define ENCODE2_H

#include "../config.h"

void HGenerator(int H_block[][bgn], int mb, int nb, int z, int** H, int** Hp, int** Hs);
void PSOHGenerator(const char *filename, int m, int n, int** H, int** Hp, int** Hs);
void Encoder(int** H, int** Hs, int** Hp, int* s, int mb, int kb, int z, int* x);

#endif
