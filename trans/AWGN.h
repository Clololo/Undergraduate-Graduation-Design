#ifndef AWGN_H
#define AWGN_H

void awgn_channel(int *int_array, double **data, int length, double snr_db);
void changeDigit(int **int_array,double *data, int length);

#endif