#!/bin/bash

# 启用错误检测（在出现错误时终止脚本）
set -e

# 编译 trainrun 可执行文件
echo "Compiling trainrun..."
gcc -o ../bin/trainrun \
    ../src/encoding/encode.c ../src/decoding/pso.c \
    ../src/trans/AWGN.c ../src/trans/electron_sim.c ../src/train.c \
    ../src/tools/utils.c ../src/tools/mat_tool.c ../src/tools/print_tool.c ../src/tools/math_tool.c \
    ../src/trans/trans.c ../src/decoding/NMS_WBP.c ../src/newencoding/bg2read.c ../src/newencoding/encode.c \
    -lm

# 检查是否编译成功
if [ $? -ne 0 ]; then
    echo "Compilation failed."
    exit 1
fi

# 切换到上级目录并运行程序
cd ..
./bin/trainrun

# 返回 scripts 目录
cd scripts

# 暂停结束（模拟 pause）
read -p "Press enter to continue..."