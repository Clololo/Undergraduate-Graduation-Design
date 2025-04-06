#!/bin/bash

# 启用错误检测
set -e

# 记录开始时间
start_time=$(date +%s%3N)

# 编译 PSOtest 可执行文件（C文件）
echo "Compiling PSOtest..."
gcc -o ../bin/PSOtest \
    ../src/PSO_OPT/psotest.c ../src/PSO_OPT/pso.c ../src/PSO_OPT/FDE.c ../src/PSO_OPT/GA.c \
    ../src/tools/print_tool.c ../src/tools/math_tool.c
if [ $? -ne 0 ]; then
    echo "Compilation of PSOtest failed."
    exit 1
fi

# 编译 peg 可执行文件（C++ 文件）
echo "Compiling peg..."
g++ -o ../bin/peg ../src/PSO_OPT/peg.cpp ../src/tools/print_tool.c ../src/tools/math_tool.c
if [ $? -ne 0 ]; then
    echo "编译 peg 失败，请检查源代码。"
    exit 1
fi

# 记录编译完成时间
compile_end_time=$(date +%s%3N)

# 切换到项目根目录运行程序
cd ..
exec_start_time=$(date +%s%3N)

echo "Running PSOtest..."
./bin/PSOtest

echo "Running peg..."
./bin/peg

exec_end_time=$(date +%s%3N)

# 切回 scripts 目录
cd scripts

# 计算并打印运行耗时
run_time=$((exec_end_time - compile_end_time))
hours=$((run_time / 3600000))
minutes=$(( (run_time % 3600000) / 60000 ))
seconds=$(( (run_time % 60000) / 1000 ))
millis=$(( run_time % 1000 ))

echo "运行耗时: ${hours} 小时 ${minutes} 分钟 ${seconds} 秒 ${millis} 毫秒"