# -*- coding: utf-8 -*-

import numpy as np
import matplotlib.pyplot as plt
import pandas as pd

def plot_ber_comparison(csv_file, output_img=None, mode=1):
    # 读取数据
    data = pd.read_csv(csv_file, header=None)
    ebno_db = data.iloc[:, 0].astype(float)
    ber_deoptimized = data.iloc[:, 1].astype(float)
    ber_original = data.iloc[:, 2].astype(float)
    ber_deenoptimized = data.iloc[:, 7].astype(float)
    ber_enoptimized = data.iloc[:, 8].astype(float)
    fer_deoptimized = data.iloc[:, 3].astype(float)
    fer_original = data.iloc[:, 4].astype(float)
    fer_deenoptimized = data.iloc[:, 9].astype(float)
    fer_enoptimized = data.iloc[:, 10].astype(float)
    print(ebno_db)
    print(ber_deoptimized)
    print(ber_original)
    
    # 创建图形
    plt.figure(figsize=(10, 6))
    plt.grid(True, which="both", ls="--", alpha=0.5)
    
    # 绘制曲线 (使用对数坐标)
    if mode == 1:
        plt.semilogy(ebno_db, ber_deoptimized, 
                    'b-o', linewidth=1, markersize=6, 
                    label='Optimized_ONLYDE LDPC(BER)')
        
        plt.semilogy(ebno_db, ber_original, 
                    color='orange', linestyle='--', marker='s', 
                    linewidth=1, markersize=6, 
                    label='Original LDPC(BER)')
        
        plt.semilogy(ebno_db, ber_deenoptimized, 
                    'g--s', linewidth=1, markersize=6,
                    label='Optimized_BOTH LDPC(BER)')
        
        plt.semilogy(ebno_db, ber_enoptimized, 
                    color='purple', linestyle='--', marker='s',
                    linewidth=1, markersize=6,
                    label='Optimized_ONLYEN LDPC(BER)')
    else:
        plt.semilogy(ebno_db, fer_deoptimized, 
                    'b-o', linewidth=1, markersize=6, 
                    label='Optimized_ONLYDE LDPC(FER)')
        
        plt.semilogy(ebno_db, fer_original, 
                    color='orange', linestyle='--', marker='s', 
                    linewidth=1, markersize=6, 
                    label='Original LDPC(FER)')
        
        plt.semilogy(ebno_db, fer_deenoptimized, 
                    'g--s', linewidth=1, markersize=6,
                    label='Optimized_BOTH LDPC(FER)')
        
        plt.semilogy(ebno_db, fer_enoptimized, 
                    color='purple', linestyle='--', marker='s',
                    linewidth=1, markersize=6,
                    label='Optimized_ONLYEN LDPC(FER)')
    
    # 设置坐标�?
    plt.xlim(0, 5)  # 根据需求调整范�?
    plt.ylim(1e-5, 1)
    plt.xlabel('Eb/N0 (dB)', fontsize=12)
    if mode == 1:
        plt.ylabel('Bit Error Rate (BER)', fontsize=12)
    else:
        plt.ylabel('Frame Error Rate (FER)', fontsize=12)
    plt.title('LDPC Performance Comparison', fontsize=14)
    
    # 特殊刻度设置
    plt.yticks([1, 1e-1, 1e-2, 1e-3, 1e-4, 1e-5],
              ["$10^{0}$", "$10^{-1}$", "$10^{-2}$", "$10^{-3}$", 
               "$10^{-4}$", "$10^{-5}$"])
    
    # 将原点�?�在左上�?
    ax = plt.gca()
    #ax.invert_xaxis()  # 如果需要横轴也反向�?以取消注�?
    
    # 添加图例和注�?
    # 调整图例位置，使其在图�??
    plt.legend(loc='upper left', bbox_to_anchor=(1.05, 1), fontsize=6)

    plt.text(0.02, 0.02, 'Lower is Better', 
            transform=ax.transAxes, fontsize=10,
            bbox=dict(facecolor='white', alpha=0.8))
    
    # 输出结果
    if output_img:
        plt.savefig(output_img, dpi=300, bbox_inches='tight')
        print(f"Plot saved to {output_img}")
    else:
        plt.show()
        

def plot_iteration_comparison(csv_file, output_img=None):
    # 读取数据 (注意列索引从0开�?)
    data = pd.read_csv(csv_file, header=None)
    ebno_db = data.iloc[:, 0].astype(float)    # �?1�?: Eb/N0
    iter_deoptimized = data.iloc[:, 5].astype(float)  # �?6�?: 优化�?代�?�数
    iter_original = data.iloc[:, 6].astype(float)   # �?7�?: 原�?�迭代�?�数
    iter_deenoptimized = data.iloc[:, 11].astype(float)  # �?6�?: 优化�?代�?�数
    iter_enoptimized = data.iloc[:, 12].astype(float)   # �?7�?: 原�?�迭代�?�数
    
    # 创建图形
    plt.figure(figsize=(10, 6))
    plt.grid(True, linestyle='--', alpha=0.6)
    
    # 绘制曲线
    plt.plot(ebno_db, iter_deoptimized, 
            'g-^', linewidth=1, markersize=6, 
            label='Optimized_ONLYDE LDPC')

    plt.plot(ebno_db, iter_original, 
            color='orange', linestyle='--', marker='D', 
            linewidth=1, markersize=6, 
            label='Original LDPC')

    plt.plot(ebno_db, iter_deenoptimized, 
            'b--D', linewidth=1, markersize=6,
            label='Optimized_BOTH LDPC')

    plt.plot(ebno_db, iter_enoptimized, 
            color='purple', linestyle='--', marker='D',
            linewidth=1, markersize=6,
            label='Optimized_ONLYEN LDPC')
        
    # 设置坐标�?
    plt.xlim(0, 5)  # �?轴范�?
    plt.ylim(0, 30)  # 纵轴�?动适应
    
    plt.xlabel('Eb/N0 (dB)', fontsize=12)
    plt.ylabel('Average Decoding Iterations', fontsize=12)
    plt.title('LDPC Decoding Complexity Comparison', fontsize=14)
    
    # 设置原点在左下�?? (默�?�就�?)
    ax = plt.gca()
    ax.set_ylim(bottom=0)  # �?保纵轴从0开�?
    
    # 添加图例和注�?
    plt.legend(loc='upper right', fontsize=6)
    plt.text(0.02, 0.98, 'Lower is Better', 
            transform=ax.transAxes, fontsize=10,
            ha='left', va='top',
            bbox=dict(facecolor='white', alpha=0.8))
    
    # 输出结果
    if output_img:
        plt.savefig(output_img, dpi=300, bbox_inches='tight')
        print(f"Plot saved to {output_img}")
    else:
        plt.show()


# 使用示例
plot_ber_comparison('C:/code/C/ldpc_emulation/output/data.csv', output_img='ldpc_comparison_ber.png', mode = 1)
plot_ber_comparison('C:/code/C/ldpc_emulation/output/data.csv', output_img='.ldpc_comparison_fer.png', mode = 2)

# 使用示例
plot_iteration_comparison('C:/code/C/ldpc_emulation/output/data.csv', output_img='iteration_comparison.png')