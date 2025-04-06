#include "LDPC.h"
#include "config.h"
#include <time.h>
#include <stdio.h>

int main(){

    clock_t start1_time = clock();
    for(double snr = 0.0; snr <= 4.0; snr += 0.5){
        run(run_frames, snr, run_max_dc_iteration, run_alpha, run_beta, usepsogenH);
    }

    clock_t end1_time = clock();
    
    // 计算运行时间
    double elapsed1_time = (double)(end1_time - start1_time) / CLOCKS_PER_SEC;    

    // 打印运行时间
    printf("total running time: %.1f s\n", elapsed1_time);
} 