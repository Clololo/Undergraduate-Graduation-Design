#include "LDPC.h"
#include "config.h"

int main(){
    for(double snr = 0.0; snr <= 3.0; snr += 0.5)
        run(run_frames, snr, run_max_dc_iteration, run_alpha, run_beta, usepsogenH);
} 