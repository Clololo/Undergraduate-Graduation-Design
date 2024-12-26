@echo off

REM ��������Դ�ļ������ɿ�ִ���ļ�
gcc -o test test.c pso.c FDE.c GA.c ../tools/math_tools.c ../tools/print_tool.c

REM ����Ƿ����ɹ�
if %errorlevel% neq 0 (
    echo Compilation failed.
    exit /b %errorlevel%
)

REM �������ɵĳ���
test.exe


rem ��� Python �Ƿ�װ
python --version >nul 2>&1
if %errorlevel% neq 0 (
    echo Error: Python is not installed or not in PATH.
    pause
    exit /b
)

rem ���� Python �ű�
python ../draw/PSO/psoOptResDrawing.py

if %errorlevel% neq 0 (
    echo Error: Failed to run Python script.
    pause
    exit /b
)

echo All tasks completed successfully.
pause