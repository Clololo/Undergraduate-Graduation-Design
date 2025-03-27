#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

// Box-Muller transform to generate Gaussian noise
double generate_gaussian_noise(double mean, double std_dev) {
    static int haveSpare = 0;
    static double rand1, rand2;

    if (haveSpare) {
        haveSpare = 0;
        return mean + std_dev * sqrt(rand1) * sin(rand2);
    }

    haveSpare = 1;

    rand1 = rand() / ((double) RAND_MAX);
    if (rand1 < 1e-100) rand1 = 1e-100;
    rand1 = -2 * log(rand1);
    rand2 = (rand() / ((double) RAND_MAX)) * 2 * M_PI;

    return mean + std_dev * sqrt(rand1) * cos(rand2);
}

void awgn_channel(int *int_array, double **data, int length, double snr_db) {
    double snr_linear = pow(10.0, snr_db / 10);
    double signal_power = 1.0; 
    double noise_power = signal_power / snr_linear;
    double noise_std_dev = sqrt(noise_power);

    *data = (double*)malloc(length * sizeof(double));
    for (int i = 0; i < length; ++i) {
        if(int_array[i] == 0){
            (*data)[i] = -0.5 + generate_gaussian_noise(0.0, noise_std_dev);
        } else {
            (*data)[i] = 0.5 + generate_gaussian_noise(0.0, noise_std_dev);
        }
    }
}

void changeDigit(int **int_array,double *data, int length){

    for(int i = 0; i < length; i++){
        (*int_array)[i] = data[i] > 0 ? 1 : 0;
    }
}