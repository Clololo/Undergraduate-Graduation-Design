#include "test.h"

int main(){
    // frame snr iteration alpha beta 
    for(int snr = 1;snr<=31;snr++)
        run(10000, snr, 50, 0.7, 0);
} 