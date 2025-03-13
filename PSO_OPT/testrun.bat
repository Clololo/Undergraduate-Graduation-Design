@echo off

REM 编译所有源文件并生成可执行文件
gcc -o ../bin/PSOtest test.c pso.c FDE.c GA.c ../tools/print_tool.c

REM 检查是否编译成功
if %errorlevel% neq 0 (
    echo Compilation failed.
    exit /b %errorlevel%
)

REM 运行生成的程序
cd ..
bin\PSOtest.exe
cd -  # 回到之前的目录


@REM rem 检查 Python 是否安装
@REM python --version >nul 2>&1
@REM if %errorlevel% neq 0 (
@REM     echo Error: Python is not installed or not in PATH.
@REM     pause
@REM     exit /b
@REM )

@REM rem 运行 Python 脚本
@REM python ../draw/PSO/psoOptResDrawing.py

@REM if %errorlevel% neq 0 (
@REM     echo Error: Failed to run Python script.
@REM     pause
@REM     exit /b
@REM )

@REM echo All tasks completed successfully.
@REM pause