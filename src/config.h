// config.h
#ifndef CONFIG_H
#define CONFIG_H

/**
 * ʵ�����в���
 * 
 */
#define run_frames 5000
#define run_snr_db 10
#define run_max_dc_iteration 50
#define run_alpha 0.7
#define run_beta 0

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
#define max_read_length 512   //���޶�ȡֵ
#define pred_vn_l 384    //Ԥ�ڱ����ڵ�����
#define pred_cn_l 128  //Ԥ��У��ڵ�����
#define init_cn_deg 6  //��ʼ�Ż�ʱ�ĳ�ʼ�ȷֲ�Ԥ��
#define init_vn_deg 3  //��ʼ�Ż�ʱ�ĳ�ʼ�ȷֲ�Ԥ��
#define pred_code_rate_lowlimit 0.75  //Ԥ�ڵ��������
#define pre_code_rate_limit 0.9  //Ԥ�ڵ��������
#define vn_deg_max (((int)(sqrt(pred_cn_l)/2))+1)
#define cn_deg_max (((int)(sqrt(pred_vn_l)))+1)
#define dim (vn_deg_max+cn_deg_max)   //�ܲ�ͬ����
#define dimlimit 128
#define alpha_penalty 10
#define beta_penalty 10

/**
 * PSO�����Ż�
 * 
 */
#define codelength 512   //Ԥ���볤
#define sizePop_de 30   //���ӹ�ģ
#define max_iteration 50  //���볢�Դ�������
#define init_fitness 0  
#define update_window 100  //ÿ����һ������λ�õ��������
#define error_punish 100  //�������ʱ��Ч��������
#define opt_rate 0.2   //�����Ż��Ĵ���ռ����������ı���


#endif 