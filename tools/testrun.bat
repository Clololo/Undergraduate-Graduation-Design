@echo off

REM ��������Դ�ļ������ɿ�ִ���ļ�
gcc -o test test.c pso.c FDE.c GA.c math_tools.c 

REM ����Ƿ����ɹ�
if %errorlevel% neq 0 (
    echo Compilation failed.
    exit /b %errorlevel%
)

REM �������ɵĳ���
test.exe


REM ��ͣ���Ա�鿴�������
pause
