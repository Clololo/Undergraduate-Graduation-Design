@echo off

REM ��������Դ�ļ������ɿ�ִ���ļ�
gcc -o ../bin/PSOtest ../src/PSO_OPT/psotest.c ../src/PSO_OPT/pso.c ../src/PSO_OPT/FDE.c ../src/PSO_OPT/GA.c ../src/tools/print_tool.c ../src/tools/math_tools.c

REM ����Ƿ����ɹ�
if %errorlevel% neq 0 (
    echo Compilation failed.
    exit /b %errorlevel%
)

REM �������ɵĳ���
cd ..
bin\PSOtest.exe
cd /scripts

@REM rem ��� Python �Ƿ�װ
@REM python --version >nul 2>&1
@REM if %errorlevel% neq 0 (
@REM     echo Error: Python is not installed or not in PATH.
@REM     pause
@REM     exit /b
@REM )

@REM rem ���� Python �ű�
@REM python ../draw/PSO/psoOptResDrawing.py

@REM if %errorlevel% neq 0 (
@REM     echo Error: Failed to run Python script.
@REM     pause
@REM     exit /b
@REM )

@REM echo All tasks completed successfully.
@REM pause