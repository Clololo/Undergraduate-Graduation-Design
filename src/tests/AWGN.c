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

void awgn_channel(double **data, int length, double snr_db) {
    // Calculate noise standard deviation from SNR
    double snr_linear = pow(10.0, snr_db / 40.0);
    double signal_power = 1.0; // Assuming signal power is normalized to 1
    double noise_power = signal_power / snr_linear;
    double noise_std_dev = sqrt(noise_power);

    // Add Gaussian noise to each element of the array
    for (int i = 0; i < length; ++i) {
        (*data)[i] += generate_gaussian_noise(0.0, noise_std_dev);
    }
}

int main() {

    int length = 50000;
    // Example usage
    double *data =(double *)malloc(length * sizeof(double));
    double *data1 =(double *)malloc(length * sizeof(double));
    //     *Row = (int*)malloc(length * sizeof(int));
    // *data = 
    for(int i = 0;i<length;i++){
        data[i] = 0.5;
    }
    for(int i = 0;i<length;i++){
        data1[i] = -0.5;
    }
    double snr_db = 60; // Example SNR in dB

    awgn_channel(&data, length, snr_db);
    awgn_channel(&data1, length, snr_db);
    // Print the noisy data
    FILE *file = fopen("channel1.txt", "w");
    if (file == NULL) {
        printf("Error opening file!\n");
        return 1;
    }

    for (int i = 0; i < length; i++) {
        fprintf(file, "%f\n", data[i]);
    }

    // Print the noisy data
    FILE *file1 = fopen("channel2.txt", "w");
    if (file1 == NULL) {
        printf("Error opening file!\n");
        return 1;
    }

    for (int i = 0; i < length; i++) {
        fprintf(file1, "%f\n", data1[i]);
    }
    return 0;
}
