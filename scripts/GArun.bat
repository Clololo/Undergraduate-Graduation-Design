@echo off

REM ��������Դ�ļ������ɿ�ִ���ļ�
gcc -o ../bin/GA_test ../src/PSO_OPT/GA.c ../src/tools/math_tool.c 

REM ����Ƿ����ɹ�
if %errorlevel% neq 0 (
    echo Compilation failed.
    exit /b %errorlevel%
)

REM �������ɵĳ���
cd ..
bin\GA_test.exe
cd -  # �ص�֮ǰ��Ŀ¼

REM ��ͣ���Ա�鿴�������
pause
