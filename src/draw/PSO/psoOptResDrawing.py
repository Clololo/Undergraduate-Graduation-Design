import numpy as np
import matplotlib.pyplot as plt
import pandas as pd

def plot_ber_comparison(csv_file, output_img=None, mode=1):
    """
    ����LDPC�Ż�ǰ�������ʶԱ�����
    
    ����:
        csv_file: ����CSV�ļ�·�����������У�
                 - ��1��: Eb/N0 (dB)
                 - ��2��: �Ż����BER
                 - ��3��: δ�Ż���BER
        output_img: ���ͼƬ·��(��ѡ)����δָ������ʾͼ��
    """
    # ��ȡ����
    data = pd.read_csv(csv_file, header=None)
    ebno_db = data.iloc[:, 0]
    ber_optimized = data.iloc[:, 1]
    ber_original = data.iloc[:, 2]
    fer_optimized = data.iloc[:, 3]
    fer_original = data.iloc[:, 4]
    
    # ����ͼ��
    plt.figure(figsize=(10, 6))
    plt.grid(True, which="both", ls="--", alpha=0.5)
    
    # �������� (ʹ�ö�������)
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
    
    # ����������
    plt.xlim(-1, 2)  # �������������Χ
    plt.ylim(1e-6, 1)
    plt.xlabel('Eb/N0 (dB)', fontsize=12)
    if mode == 1:
        plt.ylabel('Bit Error Rate (BER)', fontsize=12)
    else:
        plt.ylabel('Frame Error Rate (FER)', fontsize=12)
    plt.title('LDPC Performance Comparison', fontsize=14)
    
    # ����̶�����
    plt.yticks([1, 1e-1, 1e-2, 1e-3, 1e-4, 1e-5, 1e-6],
              ["$10^{0}$", "$10^{-1}$", "$10^{-2}$", "$10^{-3}$", 
               "$10^{-4}$", "$10^{-5}$", "$10^{-6}$"])
    
    # ��ԭ���������Ͻ�
    ax = plt.gca()
    ax.invert_xaxis()  # �����Ҫ����Ҳ�������ȡ��ע��
    
    # ���ͼ����ע��
    plt.legend(loc='upper right', fontsize=12)
    plt.text(0.02, 0.02, 'Lower is Better', 
            transform=ax.transAxes, fontsize=10,
            bbox=dict(facecolor='white', alpha=0.8))
    
    # ������
    if output_img:
        plt.savefig(output_img, dpi=300, bbox_inches='tight')
        print(f"Plot saved to {output_img}")
    else:
        plt.show()
        

def plot_iteration_comparison(csv_file, output_img=None):
    """
    ����LDPC�Ż�ǰ��������������Ա�����
    
    ����:
        csv_file: ����CSV�ļ�·�����������У�
                 - ��1��: Eb/N0 (dB)
                 - ��6��: �Ż����ƽ����������
                 - ��7��: δ�Ż���ƽ����������
        output_img: ���ͼƬ·��(��ѡ)����δָ������ʾͼ��
    """
    # ��ȡ���� (ע����������0��ʼ)
    data = pd.read_csv(csv_file, header=None)
    ebno_db = data.iloc[:, 0]    # ��1��: Eb/N0
    iter_optimized = data.iloc[:, 5]  # ��6��: �Ż���������
    iter_original = data.iloc[:, 6]   # ��7��: ԭʼ��������
    
    # ����ͼ��
    plt.figure(figsize=(10, 6))
    plt.grid(True, linestyle='--', alpha=0.6)
    
    # ��������
    plt.plot(ebno_db, iter_optimized, 
            'g-^', linewidth=2, markersize=8, 
            label='Optimized LDPC')
    plt.plot(ebno_db, iter_original, 
            'm--D', linewidth=2, markersize=8,
            label='Original LDPC')
    
    # ����������
    plt.xlim(-1, 2)  # ���᷶Χ
    plt.ylim(0, max(max(iter_optimized), max(iter_original)) * 1.1)  # �����Զ���Ӧ
    
    plt.xlabel('Eb/N0 (dB)', fontsize=12)
    plt.ylabel('Average Decoding Iterations', fontsize=12)
    plt.title('LDPC Decoding Complexity Comparison', fontsize=14)
    
    # ����ԭ�������½� (Ĭ�Ͼ���)
    ax = plt.gca()
    ax.set_ylim(bottom=0)  # ȷ�������0��ʼ
    
    # ���ͼ����ע��
    plt.legend(loc='upper right', fontsize=12)
    plt.text(0.02, 0.98, 'Lower is Better', 
            transform=ax.transAxes, fontsize=10,
            ha='left', va='top',
            bbox=dict(facecolor='white', alpha=0.8))
    
    # ������
    if output_img:
        plt.savefig(output_img, dpi=300, bbox_inches='tight')
        print(f"Plot saved to {output_img}")
    else:
        plt.show()


# ʹ��ʾ��
plot_ber_comparison('../../output/er_results.csv', output_img='../../img/ldpc_comparison.png', mode = 1)
plot_ber_comparison('../../output/er_results.csv', output_img='../../img/ldpc_comparison.png', mode = 2)

# ʹ��ʾ��
plot_iteration_comparison('../../output/decoding_results.csv', output_img='../../img/iteration_comparison.png')