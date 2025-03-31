#include "test.h"
#include "config.h"

int main(){
    // frame snr iteration alpha beta 
    for(double snr = 1.0; snr <= 3.0; snr += 0.5)
        run(run_frames, snr, run_max_dc_iteration, run_alpha, run_beta);
} 