@echo off

REM ��������Դ�ļ������ɿ�ִ���ļ�
gcc -o ../../bin/GA_test GA.c math_tools.c 

REM ����Ƿ����ɹ�
if %errorlevel% neq 0 (
    echo Compilation failed.
    exit /b %errorlevel%
)

REM �������ɵĳ���
cd ..
cd ..
bin\GA_test.exe
cd -  # �ص�֮ǰ��Ŀ¼
cd -

REM ��ͣ���Ա�鿴�������
pause
