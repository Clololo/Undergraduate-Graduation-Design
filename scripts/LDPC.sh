#!/bin/bash

# ���ô����⣨����ͣ��
set -e

# ���� LDPCrun ��ִ���ļ�
echo "Compiling LDPCrun..."
gcc -o ../bin/LDPCrun \
    ../src/encoding/encode.c ../src/decoding/pso.c ../src/decoding/MinSumBP.c \
    ../src/trans/AWGN.c ../src/trans/electron_sim.c ../src/LDPC.c ../src/test.c \
    ../src/tools/utils.c ../src/tools/mat_tool.c ../src/tools/print_tool.c ../src/tools/math_tool.c \
    ../src/trans/trans.c ../src/decoding/NMS_WBP.c ../src/newencoding/bg2read.c ../src/newencoding/encode.c \
    -lm

# ����Ƿ����ɹ�
if [ $? -ne 0 ]; then
    echo "Compilation failed."
    exit 1
fi

# �л����ϼ�Ŀ¼���г���
cd ..
./bin/LDPCrun

# �ص� scripts Ŀ¼
cd scripts

# ģ�� Windows �� pause
read -p "Press Enter to continue..."