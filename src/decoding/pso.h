#ifndef PSO_H
#define PSO_H

#define codelength 512
#define sizePop 30
#define max_iteration 50
#define init_fitness 0
#define update_window 100

//�����Ż��Ĵ���ռ����������ı���
#define opt_rate 0.2  

extern int iters_tmp;

// ��ȡ����Ȩ��
double getWeight();
// ��ȡѧϰ����
void getLearningRate(double lr[2]);
// ����λ�÷�Χ���Զ�����
void getRangePop(double rangePop[2]);
// �����ٶȷ�Χ
void getRangeSpeed(double rangeSpeed[2]);
//��ʧ����
// ��ʼ����Ⱥ���ٶȺ���Ӧ��
void initPopVFit(int size, int codeLen, double pop[][codelength], double v[][codelength], double fitness[]);
// ��ȡ��ʼ����ֵ
void getInitBest(int size, int codeLen, double pop[][codelength], double v[][codelength], const double fitness[], double gbestPop[], double *gbestFitness,
    double pbestPop[][codelength], double pbestFitness[]);
//PSO����
double getDeclineRate(int iter,int now);
// ����Ⱥ�Ż����º�����������ʧ����������ũ���޵Ĳ�ֵ��ԽСԽ��
void update_particles(int size, int codeLen, double pop[][codelength], double v[][codelength], double fitness[],
    double pbestPop[][codelength], double pbestFitness[], double gbestPop[], double *gbestFitness);

#endif

