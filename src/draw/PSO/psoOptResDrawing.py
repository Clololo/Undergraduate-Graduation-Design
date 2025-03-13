# -*- coding: utf-8 -*-

import pandas as pd
import matplotlib.pyplot as plt

# 读取 CSV 文件
data = pd.read_csv("PSO/results.csv")

# 绘制 Rho 分布
plt.figure(figsize=(10, 5))
plt.bar(data["Degree"], data["Rho"], label="Rho Distribution", alpha=0.7)
plt.xlabel("Degree")
plt.ylabel("Probability")
plt.title("Rho Distribution")
plt.legend()
plt.savefig("rho_distribution.png")  # 保存 Rho 分布图
plt.show()

# 绘制 Lambda 分布
plt.figure(figsize=(10, 5))
plt.bar(data["Degree"], data["Lambda"], label="Lambda Distribution", alpha=0.7, color="orange")
plt.xlabel("Degree")
plt.ylabel("Probability")
plt.title("Lambda Distribution")
plt.legend()
plt.savefig("lambda_distribution.png")  # 保存 Lambda 分布图
plt.show()
