// config.h
#ifndef CONFIG_H
#define CONFIG_H

/**
 * ʵ�����в���
 * 
 */
#define run_frames 1000
#define run_max_dc_iteration 50
#define run_alpha 0.7
#define run_beta 0

//��׼ѭ����λ��������
#define checknodeNum 72   //*SubmatrixSizeΪʵ�ʳ���
#define SymbolnodeNum 144
#define SubmatrixSize 1   //��ʹ��PSO�Ż�У�����ʱ��������Ϊ1
#define gap 4

#define bgm 18
#define bgn 36
#define bgz 4
/**
 * PSO�����Ż�
 * 
 */
#define EPSILON 1e-6      // ������ֵ
#define STEP_SIZE 0.1     // ������˹�任����
#define SIGMA_TARGET 0.8  // ��ʼģ������ȣ��ɸ�����ũ���޵�����  
#define PE_THRESHOLD 1e-5
#define SIGMA_INC 1e-3
#define GA_MODE 1   
#define DE_MODE 2   
#define maxGen_en 200  //PSO�Ż�������������
#define sizePop_en 30  //���ӹ�ģ
#define MAX_ITER 800   //��˹���Ƽ���SNR��ֵʱ������������
#define max_read_length 144   //���޶�ȡֵ
#define pred_vn_l 72    //Ԥ�ڱ����ڵ�����
#define pred_cn_l 72  //Ԥ��У��ڵ�����
#define init_cn_deg 4  //��ʼ�Ż�ʱ�ĳ�ʼ�ȷֲ�Ԥ��
#define init_vn_deg 2  //��ʼ�Ż�ʱ�ĳ�ʼ�ȷֲ�Ԥ��
#define pred_code_rate_lowlimit 0.49  //Ԥ�ڵ��������
#define pre_code_rate_limit 0.6  //Ԥ�ڵ��������
#define vn_deg_max (((int)(sqrt(pred_cn_l)/2))+1)
#define cn_deg_max (((int)(sqrt(pred_vn_l)))+1)
#define dim (vn_deg_max+cn_deg_max)   //�ܲ�ͬ����
#define dimlimit 144
#define alpha_penalty 10
#define beta_penalty 10
//����ѡ��
#define en_lr0 0.49
#define en_lr1 0.69
#define en_rangepoplow 0.0
#define en_rangepophigh 1.0
#define en_rangevlow -0.03
#define en_rangevhigh 0.03
/**
 * PSO�����Ż�
 * 
 */
#define codelength 144   //Ԥ���볤
#define sizePop_de 30   //���ӹ�ģ
#define max_iteration 50  //���볢�Դ�������
#define init_fitness 0  
#define update_window 20  //ÿ����һ������λ�õ��������
#define error_punish 100  //�������ʱ��Ч��������
#define opt_rate 0.2   //�����Ż��Ĵ���ռ����������ı���
//����ѡ��
#define de_lr0 0.89
#define de_lr1 1.89
#define de_rangepoplow 0.0
#define de_rangepophigh 1.0
#define de_rangevlow -0.1
#define de_rangevhigh 0.1

#endif 