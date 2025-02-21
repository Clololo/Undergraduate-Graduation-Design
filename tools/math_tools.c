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

// // �Զ��庯������׼��̬�ֲ������ۻ��ֲ�������CDF�ķ�������
// double phi_inverse(double x) {
//    // printf("res:%lf\n",erfcinv(2 * (1 - x)));
//     return sqrt(2.0) * erfcinv(2 * (1 - x));
// }

// // �Զ��庯����phi��������׼��̬�ֲ���CDF��
// double phi(double x) {
//     return 0.5 * erfc(-x / sqrt(2.0));
// }


// phi������������̬�ֲ����ۻ��ֲ����� (CDF)
double phi(double x) {
    if (x >= 0 && x <= 10) {
        return 0.5 * exp(-0.4527 * pow(x, 0.86) + 0.0218);
    } else {
        return sqrt(M_PI / x) * exp(-x / 4) * (1 - 10 / (7 * x));
    }
}

// ����phi�ĵ���
double derivative_phi(double x) {
    if (x >= 0 && x <= 10) {
        // ���� [0, 10] �ڵĵ���
        return -0.4527 * 0.86 * pow(x, -0.14) * phi(x);
    } else {
        // ���� [10, ��) �ڵĵ���
        double term1 = exp(-x / 4) * sqrt(M_PI / x) * (-0.5 / x * (1 - 10 / (7 * x)));
        double term2 = exp(-x / 4) * sqrt(M_PI / x) * (-0.25 * (1 - 10 / (7 * x)));
        double term3 = exp(-x / 4) * sqrt(M_PI / x) * (10 / (7 * x * x));
        return term1 + term2 + term3;
    }
}
    
// phi_inverse�������������yʱ��xֵ��ʹ��phi(x) = y
double phi_inverse(double y) {3
    printf("begin pi\n");
    double x = 0.0;
    int num_iter = 0;
    if (y <= 1.0221 && y >= 0.0388) {
        // ʹ�ý�����ʽ���
        x = pow((0.0218 - log(y)) / 0.4527, 1 / 0.86);
        num_iter = 0;
    } else {
        // ʹ��ţ�ٷ������
        double x0 = 0.0388;
        double x1 = x0 - (phi(x0) - y) / derivative_phi(x0);
        double delta = fabs(x1 - x0);
        double epsilon = 1e-3;
        num_iter = 0;

        while (delta >= epsilon) {
            (num_iter)++;
            x0 = x1;
            x1 = x1 - (phi(x1) - y) / derivative_phi(x1);
            printf("%f %f ",x1,(phi(x1) - y) / derivative_phi(x1));
            if (x1 > 1e1) {
                epsilon = 0.1;
            }
            delta = fabs(x1 - x0);
            printf("delta = %f\n",delta);
        }
        x = x1;
    }
    return x;
}

