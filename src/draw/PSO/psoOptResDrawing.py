import numpy as np
import matplotlib.pyplot as plt
import pandas as pd

def plot_ber_comparison(csv_file, output_img=None, mode=1):
    """
    绘制LDPC优化前后误码率对比曲线
    
    参数:
        csv_file: 输入CSV文件路径，包含三列：
                 - 第1列: Eb/N0 (dB)
                 - 第2列: 优化后的BER
                 - 第3列: 未优化的BER
        output_img: 输出图片路径(可选)，如未指定则显示图像
    """
    # 读取数据
    data = pd.read_csv(csv_file, header=None)
    ebno_db = data.iloc[:, 0]
    ber_optimized = data.iloc[:, 1]
    ber_original = data.iloc[:, 2]
    fer_optimized = data.iloc[:, 3]
    fer_original = data.iloc[:, 4]
    
    # 创建图形
    plt.figure(figsize=(10, 6))
    plt.grid(True, which="both", ls="--", alpha=0.5)
    
    # 绘制曲线 (使用对数坐标)
    if mode == 1:
        plt.semilogy(ebno_db, ber_optimized, 
                    'b-o', linewidth=2, markersize=8, 
                    label='Optimized LDPC(BER)')
        plt.semilogy(ebno_db, ber_original, 
                    'r--s', linewidth=2, markersize=8,
                    label='Original LDPC(BER)')
    else:
        plt.semilogy(ebno_db, fer_optimized, 
                    'b--o', linewidth=2, markersize=8,
                    label='Optimized LDPC(FER)')
        plt.semilogy(ebno_db, fer_original, 
                    'r--s', linewidth=2, markersize=8,
                    label='Original LDPC(FER)')
    
    # 设置坐标轴
    plt.xlim(-1, 2)  # 根据需求调整范围
    plt.ylim(1e-6, 1)
    plt.xlabel('Eb/N0 (dB)', fontsize=12)
    if mode == 1:
        plt.ylabel('Bit Error Rate (BER)', fontsize=12)
    else:
        plt.ylabel('Frame Error Rate (FER)', fontsize=12)
    plt.title('LDPC Performance Comparison', fontsize=14)
    
    # 特殊刻度设置
    plt.yticks([1, 1e-1, 1e-2, 1e-3, 1e-4, 1e-5, 1e-6],
              ["$10^{0}$", "$10^{-1}$", "$10^{-2}$", "$10^{-3}$", 
               "$10^{-4}$", "$10^{-5}$", "$10^{-6}$"])
    
    # 将原点设在左上角
    ax = plt.gca()
    ax.invert_xaxis()  # 如果需要横轴也反向可以取消注释
    
    # 添加图例和注释
    plt.legend(loc='upper right', fontsize=12)
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
    """
    绘制LDPC优化前后译码迭代次数对比曲线
    
    参数:
        csv_file: 输入CSV文件路径，包含多列：
                 - 第1列: Eb/N0 (dB)
                 - 第6列: 优化后的平均迭代次数
                 - 第7列: 未优化的平均迭代次数
        output_img: 输出图片路径(可选)，如未指定则显示图像
    """
    # 读取数据 (注意列索引从0开始)
    data = pd.read_csv(csv_file, header=None)
    ebno_db = data.iloc[:, 0]    # 第1列: Eb/N0
    iter_optimized = data.iloc[:, 5]  # 第6列: 优化迭代次数
    iter_original = data.iloc[:, 6]   # 第7列: 原始迭代次数
    
    # 创建图形
    plt.figure(figsize=(10, 6))
    plt.grid(True, linestyle='--', alpha=0.6)
    
    # 绘制曲线
    plt.plot(ebno_db, iter_optimized, 
            'g-^', linewidth=2, markersize=8, 
            label='Optimized LDPC')
    plt.plot(ebno_db, iter_original, 
            'm--D', linewidth=2, markersize=8,
            label='Original LDPC')
    
    # 设置坐标轴
    plt.xlim(-1, 2)  # 横轴范围
    plt.ylim(0, max(max(iter_optimized), max(iter_original)) * 1.1)  # 纵轴自动适应
    
    plt.xlabel('Eb/N0 (dB)', fontsize=12)
    plt.ylabel('Average Decoding Iterations', fontsize=12)
    plt.title('LDPC Decoding Complexity Comparison', fontsize=14)
    
    # 设置原点在左下角 (默认就是)
    ax = plt.gca()
    ax.set_ylim(bottom=0)  # 确保纵轴从0开始
    
    # 添加图例和注释
    plt.legend(loc='upper right', fontsize=12)
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
plot_ber_comparison('../../output/er_results.csv', output_img='../../img/ldpc_comparison.png', mode = 1)
plot_ber_comparison('../../output/er_results.csv', output_img='../../img/ldpc_comparison.png', mode = 2)

# 使用示例
plot_iteration_comparison('../../output/decoding_results.csv', output_img='../../img/iteration_comparison.png')