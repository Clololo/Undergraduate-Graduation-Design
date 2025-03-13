#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

double generate_normal(double mean, double stddev) {
    static const double epsilon = 1e-10;
    static const double two_pi = 2.0 * 3.14159265358979323846;

    static double z0, z1;
    static int generate;
    generate = !generate;

    if (!generate)
        return z1 * stddev + mean;

    double u1, u2;
    do {
        u1 = rand() * (1.0 / RAND_MAX);
        u2 = rand() * (1.0 / RAND_MAX);
    } while (u1 <= epsilon);

    z0 = sqrt(-2.0 * log(u1)) * cos(two_pi * u2);
    z1 = sqrt(-2.0 * log(u1)) * sin(two_pi * u2);

    return z0 * stddev + mean;
}

void convert_array(int *int_array, double *double_array, int length, double mean, double stddev) {
    for (int i = 0; i < length; i++) {
        if (int_array[i] == 0) {
            double_array[i] = generate_normal(mean-1, stddev);
            // if (double_array[i] < -1.0) {
            //     double_array[i] = -1.0;
            // } else if (double_array[i] > 0.0) {
            //     double_array[i] = 0.0;
            // }
        } else {
            double_array[i] = generate_normal(mean, stddev);
            // if (double_array[i] < 0.0) {
            //     double_array[i] = 0.0;
            // } else if (double_array[i] > 1.0) {
            //     double_array[i] = 1.0;
            // }
        }
    }
}

int main() {
    int length = 50000;
    int int_array[length];
    double double_array[length];
    double double_array2[length];
    srand(time(NULL));
    for (int i = 0; i < length; i++) {
        int_array[i] = 0;
    }

    convert_array(int_array, double_array, length);

    for (int i = 0; i < length; i++) {
        int_array[i] = 1;
    }

    convert_array(int_array, double_array2, length);

    FILE *file = fopen("double_array1.txt", "w");
    if (file == NULL) {
        printf("Error opening file!\n");
        return 1;
    }

    for (int i = 0; i < length; i++) {
        fprintf(file, "%f\n", double_array[i]);
    }

    fclose(file);

    FILE *file1 = fopen("double_array2.txt", "w");
    if (file == NULL) {
        printf("Error opening file!\n");
        return 1;
    }

    for (int i = 0; i < length; i++) {
        fprintf(file, "%f\n", double_array2[i]);
    }

    fclose(file);
    return 0;
}
