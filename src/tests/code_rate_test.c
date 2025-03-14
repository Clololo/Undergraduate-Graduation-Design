#include<stdio.h>

double rho[1010];
double lambda[1010];

int cn_deg_max = 10;
int vn_deg_max = 10;
double compute_code_rate() {    

    double sum_rho = 0.0, sum_lambda = 0.0;
    for (int j = 1; j <= cn_deg_max; j++) {  
        sum_rho += rho[j] * (double)j;    
    }   
    for (int i = 1; i <= vn_deg_max; i++) {
        sum_lambda += lambda[i] * (double)i;
    }

    // ¼ÆËãÂëÂÊ R
    double R = 1.0 - (sum_lambda / sum_rho);
    return R;
}

int main(){
    rho[5] = 2.0/3.0;
    rho[6] = 1.0/3.0;
    lambda[2] = 5.0/7.0;
    lambda[3] = 2.0/7.0;
    printf("cr = %f\n",compute_code_rate());
}