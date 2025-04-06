#!/bin/bash

# ���ô�����
set -e

# ��¼��ʼʱ��
start_time=$(date +%s%3N)

# ���� PSOtest ��ִ���ļ���C�ļ���
echo "Compiling PSOtest..."
gcc -o ../bin/PSOtest \
    ../src/PSO_OPT/psotest.c ../src/PSO_OPT/pso.c ../src/PSO_OPT/FDE.c ../src/PSO_OPT/GA.c \
    ../src/tools/print_tool.c ../src/tools/math_tool.c
if [ $? -ne 0 ]; then
    echo "Compilation of PSOtest failed."
    exit 1
fi

# ���� peg ��ִ���ļ���C++ �ļ���
echo "Compiling peg..."
g++ -o ../bin/peg ../src/PSO_OPT/peg.cpp ../src/tools/print_tool.c ../src/tools/math_tool.c
if [ $? -ne 0 ]; then
    echo "���� peg ʧ�ܣ�����Դ���롣"
    exit 1
fi

# ��¼�������ʱ��
compile_end_time=$(date +%s%3N)

# �л�����Ŀ��Ŀ¼���г���
cd ..
exec_start_time=$(date +%s%3N)

echo "Running PSOtest..."
./bin/PSOtest

echo "Running peg..."
./bin/peg

exec_end_time=$(date +%s%3N)

# �л� scripts Ŀ¼
cd scripts

# ���㲢��ӡ���к�ʱ
run_time=$((exec_end_time - compile_end_time))
hours=$((run_time / 3600000))
minutes=$(( (run_time % 3600000) / 60000 ))
seconds=$(( (run_time % 60000) / 1000 ))
millis=$(( run_time % 1000 ))

echo "���к�ʱ: ${hours} Сʱ ${minutes} ���� ${seconds} �� ${millis} ����"