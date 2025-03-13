@echo off

REM ��������Դ�ļ������ɿ�ִ���ļ�
gcc -o ../bin/PSOtest test.c pso.c FDE.c GA.c ../tools/print_tool.c

REM ����Ƿ����ɹ�
if %errorlevel% neq 0 (
    echo Compilation failed.
    exit /b %errorlevel%
)

REM �������ɵĳ���
cd ..
bin\PSOtest.exe
cd -  # �ص�֮ǰ��Ŀ¼


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