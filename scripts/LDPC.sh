#!/bin/bash

# 启用错误检测（遇错即停）
set -e

# 编译 LDPCrun 可执行文件
echo "Compiling LDPCrun..."
gcc -o ../bin/LDPCrun \
    ../src/encoding/encode.c ../src/decoding/pso.c ../src/decoding/MinSumBP.c \
    ../src/trans/AWGN.c ../src/trans/electron_sim.c ../src/LDPC.c ../src/test.c \
    ../src/tools/utils.c ../src/tools/mat_tool.c ../src/tools/print_tool.c ../src/tools/math_tool.c \
    ../src/trans/trans.c ../src/decoding/NMS_WBP.c ../src/newencoding/bg2read.c ../src/newencoding/encode.c \
    -lm

# 检查是否编译成功
if [ $? -ne 0 ]; then
    echo "Compilation failed."
    exit 1
fi

# 切换到上级目录运行程序
cd ..
./bin/LDPCrun

# 回到 scripts 目录
cd scripts

# 模拟 Windows 的 pause
read -p "Press Enter to continue..."