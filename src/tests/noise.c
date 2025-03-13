#include <stdio.h>
#include <math.h>

double calculate_noise_db(double snr_db) {
    double noise_power = pow(10.0, snr_db / 30.0); // 噪声功率
    double noise_power_db = 10 * log10(noise_power); // 噪声功率的分贝表示
    return noise_power_db;
}

int main() {
    double snr_db = 20.0; // 示例SNR值（分贝）
    double noise_db = calculate_noise_db(snr_db);
    printf("实际噪声分贝: %.2f dB\n", noise_db);
    return 0;
}
