@echo off

REM ��������Դ�ļ������ɿ�ִ���ļ�
g++ -o ../bin/peg ../src/PSO_OPT/peg.cpp ../src/tools/print_tool.c ../src/tools/math_tool.c

:: �������Ƿ�ɹ�
if %errorlevel% neq 0 (
    echo ����ʧ�ܣ�����Դ���롣
    pause
    exit /b %errorlevel%
)

REM �������ɵĳ���
cd ..
bin\peg.exe
cd scripts