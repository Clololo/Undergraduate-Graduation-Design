#!/bin/bash

# ���ô����⣨�ڳ��ִ���ʱ��ֹ�ű���
set -e

# ���� trainrun ��ִ���ļ�
echo "Compiling trainrun..."
gcc -o ../bin/trainrun \
    ../src/encoding/encode.c ../src/decoding/pso.c \
    ../src/trans/AWGN.c ../src/trans/electron_sim.c ../src/train.c \
    ../src/tools/utils.c ../src/tools/mat_tool.c ../src/tools/print_tool.c ../src/tools/math_tool.c \
    ../src/trans/trans.c ../src/decoding/NMS_WBP.c ../src/newencoding/bg2read.c ../src/newencoding/encode.c \
    -lm

# ����Ƿ����ɹ�
if [ $? -ne 0 ]; then
    echo "Compilation failed."
    exit 1
fi

# �л����ϼ�Ŀ¼�����г���
cd ..
./bin/trainrun

# ���� scripts Ŀ¼
cd scripts

# ��ͣ������ģ�� pause��
read -p "Press enter to continue..."