@echo off

REM ��������Դ�ļ������ɿ�ִ���ļ�
gcc -o FDE_test test.c FDE.c pso.c

REM ����Ƿ����ɹ�
if %errorlevel% neq 0 (
    echo Compilation failed.
    exit /b %errorlevel%
)

REM �������ɵĳ���
main.exe

REM ��ͣ���Ա�鿴�������
pause
