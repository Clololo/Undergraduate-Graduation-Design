#ifndef MATH_TOOL_H
#define MATH_TOOL_H

double erfcinv(double p);
double normcdf(double x);
double phi_inverse(double x);
double phi(double x);

// 近似算法：逆误差函数 erfcinv
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

double erfcinv(double p) {
    // Check bounds
    if (p <= 0.0 || p >= 2.0) {
       // printf("erfcinv argument out of range\n");
        return NAN;
    }

    double x, z;
    if (p < 1.0) {
        z = p;
    } else {
        z = 2.0 - p;
    }

    // Coefficients for approximation
    double a[] = {  0.000000000000000000000000000000,  0.0705230784, 0.0422820123,
                    0.0092705272, 0.0001520143, 0.0002765672, 0.0000430638, 0.0000083144 };
    double b[] = {  0.0030307722, 0.0054044081, 0.0029710203, 0.0004478303, 0.0000148693 };

    // Using rational approximation from the paper
    x = sqrt(-2.0 * log(z / 2.0));
    return (x - a[0] - a[1]*x - a[2]*x*x - a[3]*x*x*x - a[4]*x*x*x*x) / (1.0 + b[0]*x + b[1]*x*x + b[2]*x*x*x);
}

// 自定义函数：标准正态分布的累积分布函数（CDF）
double normcdf(double x) {
    return 0.5 * erfc(-x / sqrt(2.0));
}

// phi函数，计算正态分布的累积分布函数 (CDF)
double phi(double x) {
    if (x >= 0 && x <= 10) {
        return exp(-0.4527 * pow(x, 0.86) + 0.0218);
    } else {
        return sqrt(M_PI / x) * exp(-x / 4) * (1 - 10 / (7 * x));
    }
}

// 计算phi的导数
double phi_derivative(double x) {
    if (x >= 0 && x <= 10) {
        return exp(-0.4527 * pow(x, 0.86) + 0.0218) * (-0.4527 * 0.86 * pow(x, -0.14));
    } else {
        double f = sqrt(M_PI / x);
        double g = exp(-x / 4);
        double h = 1 - 10.0 / (7 * x);
        
        double f_prime = -0.5 * sqrt(M_PI) * pow(x, -1.5);
        double g_prime = -0.25 * g;
        double h_prime = 10.0 / (7 * x * x);
        
        return f_prime * g * h + f * g_prime * h + f * g * h_prime;
    }
}

    
// phi_inverse函数，计算给定y时的x值，使得phi(x) = y
double phi_inverse(double y) {
    //printf("begin pi\n");
    double x = 0.0;
    int num_iter = 0;
    if (y <= 1.0221 && y >= 0.0388) {
        // 使用解析公式求解
        x = pow((0.0218 - log(y)) / 0.4527, 1 / 0.86);
        num_iter = 0;
    } else {
        // 使用牛顿法来求解
        //double x0 = 0.0388; 
        double x0 = (y < 0.0388) ? 10.0 : 0.0388;  // 根据 y 选择合适的初始值
        //double x0 = 0.0388;
        double x1 = x0 - (phi(x0) - y) / phi_derivative(x0);
        double delta = fabs(x1 - x0);
        double epsilon = 1e-5;
        num_iter = 0;
        int max_iter = 200;

        while (delta >= epsilon && num_iter < max_iter) {
            (num_iter)++;
            x0 = x1;
            double phi_x1 = phi(x1);
            double phi_der_x1 = phi_derivative(x1);

            if(fabs(phi_der_x1) < 1e-8){
                break;
            }
            x1 = x1 - (phi_x1 - y) / phi_der_x1;
            delta = fabs(x1 - x0);
           // printf("Iteration %d: x = %f, delta = %f\n", num_iter, x1, delta);

            if (num_iter >= max_iter) {
                printf("Warning: Newton's method did not fully converge.\n");
            }
        }
        x = x1;
    }
    return x;
}



#endif