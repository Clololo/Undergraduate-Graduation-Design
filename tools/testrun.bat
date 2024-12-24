@echo off

REM 编译所有源文件并生成可执行文件
gcc -o test test.c pso.c FDE.c GA.c math_tools.c 

REM 检查是否编译成功
if %errorlevel% neq 0 (
    echo Compilation failed.
    exit /b %errorlevel%
)

REM 运行生成的程序
test.exe


REM 暂停，以便查看程序输出
pause
