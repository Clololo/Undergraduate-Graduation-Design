@echo off

REM ��������Դ�ļ������ɿ�ִ���ļ�
gcc -o ../bin/FDE_test ../src/PSO_OPT/test.c ../src/PSO_OPT/FDE.c ../src/PSO_OPT/pso.c

REM ����Ƿ����ɹ�
if %errorlevel% neq 0 (
    echo Compilation failed.
    exit /b %errorlevel%
)

REM �������ɵĳ���
cd ..
bin\FDE_test.exe
cd -  # �ص�֮ǰ��Ŀ¼

REM ��ͣ���Ա�鿴�������
pause
