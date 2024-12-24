// �����㷨�������� erfcinv
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

// �Զ��庯������׼��̬�ֲ����ۻ��ֲ�������CDF��
double normcdf(double x) {
    return 0.5 * erfc(-x / sqrt(2.0));
}

// �Զ��庯������׼��̬�ֲ������ۻ��ֲ�������CDF�ķ�������
double phi_inverse(double x) {
    return sqrt(2.0) * erfcinv(2 * (1 - x));
}

// �Զ��庯����phi��������׼��̬�ֲ���CDF��
double phi(double x) {
    return 0.5 * erfc(-x / sqrt(2.0));
}
