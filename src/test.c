#include "test.h"
#include "config.h"

int main(){
    // frame snr iteration alpha beta 
    // for(int snr = 1;snr<=31;snr++)
    //     run(10000, snr, 50, 0.7, 0);
    run(run_frames, run_snr_db, run_max_dc_iteration, run_alpha, run_beta);
} 