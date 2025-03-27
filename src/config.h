// config.h
#ifndef CONFIG_H
#define CONFIG_H

/**
 * 实际运行参数
 * 
 */
#define run_frames 5000
#define run_snr_db 10
#define run_max_dc_iteration 50
#define run_alpha 0.7
#define run_beta 0

/**
 * PSO编码优化
 * 
 */
#define EPSILON 1e-6      // 收敛阈值
#define STEP_SIZE 0.1     // 拉普拉斯变换步长
#define SIGMA_TARGET 0.8  // 初始模拟信噪比（可根据香农极限调整）  
#define PE_THRESHOLD 1e-5
#define SIGMA_INC 1e-3
#define GA_MODE 1   
#define DE_MODE 2   
#define maxGen_en 200  //PSO优化迭代次数上限
#define sizePop_en 30  //粒子规模
#define MAX_ITER 800   //高斯近似计算SNR阈值时的最大迭代次数
#define max_read_length 512   //上限读取值
#define pred_vn_l 384    //预期变量节点数量
#define pred_cn_l 128  //预期校验节点数量
#define init_cn_deg 6  //开始优化时的初始度分布预设
#define init_vn_deg 3  //开始优化时的初始度分布预设
#define pred_code_rate_lowlimit 0.75  //预期的最低码率
#define pre_code_rate_limit 0.9  //预期的最高码率
#define vn_deg_max (((int)(sqrt(pred_cn_l)/2))+1)
#define cn_deg_max (((int)(sqrt(pred_vn_l)))+1)
#define dim (vn_deg_max+cn_deg_max)   //总不同度数
#define dimlimit 128
#define alpha_penalty 10
#define beta_penalty 10

/**
 * PSO译码优化
 * 
 */
#define codelength 512   //预期码长
#define sizePop_de 30   //粒子规模
#define max_iteration 50  //译码尝试次数上限
#define init_fitness 0  
#define update_window 100  //每更新一次粒子位置的译码次数
#define error_punish 100  //译码出错时等效迭代次数
#define opt_rate 0.2   //用于优化的次数占总译码次数的比例


#endif 