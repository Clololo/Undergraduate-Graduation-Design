@echo off

REM 编译所有源文件并生成可执行文件
gcc -o ../bin/FDE_test ../src/PSO_OPT/test.c ../src/PSO_OPT/FDE.c ../src/PSO_OPT/pso.c

REM 检查是否编译成功
if %errorlevel% neq 0 (
    echo Compilation failed.
    exit /b %errorlevel%
)

REM 运行生成的程序
cd ..
bin\FDE_test.exe
cd -  # 回到之前的目录

REM 暂停，以便查看程序输出
pause
