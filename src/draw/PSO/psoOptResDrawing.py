# -*- coding: utf-8 -*-

import pandas as pd
import matplotlib.pyplot as plt

# ��ȡ CSV �ļ�
data = pd.read_csv("PSO/results.csv")

# ���� Rho �ֲ�
plt.figure(figsize=(10, 5))
plt.bar(data["Degree"], data["Rho"], label="Rho Distribution", alpha=0.7)
plt.xlabel("Degree")
plt.ylabel("Probability")
plt.title("Rho Distribution")
plt.legend()
plt.savefig("rho_distribution.png")  # ���� Rho �ֲ�ͼ
plt.show()

# ���� Lambda �ֲ�
plt.figure(figsize=(10, 5))
plt.bar(data["Degree"], data["Lambda"], label="Lambda Distribution", alpha=0.7, color="orange")
plt.xlabel("Degree")
plt.ylabel("Probability")
plt.title("Lambda Distribution")
plt.legend()
plt.savefig("lambda_distribution.png")  # ���� Lambda �ֲ�ͼ
plt.show()
