// phi函数，计算正态分布的累积分布函数 (CDF)
#include<math.h>
#include<stdio.h>

double phi(double x) {
    if (x >= 0 && x <= 10) {
        return 0.5 * exp(-0.4527 * pow(x, 0.86) + 0.0218);
    } else {
        return sqrt(M_PI / x) * exp(-x / 4) * (1 - 10 / (7 * x));
    }
}

double phi_1(double x) {
    //return 0.5 * (1.0 + erf(x / sqrt(2.0)));
     return 0.5 * erfc(-x / sqrt(2.0));
}

int main(){
    for(double i = 0;i<=20;i+=0.1){
        printf("%f  %.10f %.10f\n",i,phi(i),phi_1(i));
    }
}


