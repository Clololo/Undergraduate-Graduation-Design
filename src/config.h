// config.h
#ifndef CONFIG_H
#define CONFIG_H

/**
 * 实际运行参数
 * 
 */
#define run_frames 1000
#define run_max_dc_iteration 50
#define run_alpha 0.7
#define run_beta 0

//标准循环移位矩阵配置
#define checknodeNum 72   //*SubmatrixSize为实际长度
#define SymbolnodeNum 144
#define SubmatrixSize 1   //在使用PSO优化校验矩阵时，这里设为1
#define gap 4

#define bgm 18
#define bgn 36
#define bgz 4
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
#define max_read_length 144   //上限读取值
#define pred_vn_l 72    //预期变量节点数量
#define pred_cn_l 72  //预期校验节点数量
#define init_cn_deg 4  //开始优化时的初始度分布预设
#define init_vn_deg 2  //开始优化时的初始度分布预设
#define pred_code_rate_lowlimit 0.49  //预期的最低码率
#define pre_code_rate_limit 0.6  //预期的最高码率
#define vn_deg_max (((int)(sqrt(pred_cn_l)/2))+1)
#define cn_deg_max (((int)(sqrt(pred_vn_l)))+1)
#define dim (vn_deg_max+cn_deg_max)   //总不同度数
#define dimlimit 144
#define alpha_penalty 10
#define beta_penalty 10
//参数选择
#define en_lr0 0.49
#define en_lr1 0.69
#define en_rangepoplow 0.0
#define en_rangepophigh 1.0
#define en_rangevlow -0.03
#define en_rangevhigh 0.03
/**
 * PSO译码优化
 * 
 */
#define codelength 144   //预期码长
#define sizePop_de 30   //粒子规模
#define max_iteration 50  //译码尝试次数上限
#define init_fitness 0  
#define update_window 20  //每更新一次粒子位置的译码次数
#define error_punish 100  //译码出错时等效迭代次数
#define opt_rate 0.2   //用于优化的次数占总译码次数的比例
//参数选择
#define de_lr0 0.89
#define de_lr1 1.89
#define de_rangepoplow 0.0
#define de_rangepophigh 1.0
#define de_rangevlow -0.1
#define de_rangevhigh 0.1

#endif 