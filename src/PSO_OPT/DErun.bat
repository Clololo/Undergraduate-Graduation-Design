@echo off

REM ��������Դ�ļ������ɿ�ִ���ļ�
gcc -o ../../bin/FDE_test test.c FDE.c pso.c

REM ����Ƿ����ɹ�
if %errorlevel% neq 0 (
    echo Compilation failed.
    exit /b %errorlevel%
)

REM �������ɵĳ���
cd ..
cd ..
bin\FDE_test.exe
cd -  # �ص�֮ǰ��Ŀ¼
cd -

REM ��ͣ���Ա�鿴�������
pause
