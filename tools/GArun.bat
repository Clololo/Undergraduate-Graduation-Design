@echo off

REM ��������Դ�ļ������ɿ�ִ���ļ�
gcc -o GA_test GA.c math_tools.c 

REM ����Ƿ����ɹ�
if %errorlevel% neq 0 (
    echo Compilation failed.
    exit /b %errorlevel%
)

REM �������ɵĳ���
main.exe

REM ��ͣ���Ա�鿴�������
pause
