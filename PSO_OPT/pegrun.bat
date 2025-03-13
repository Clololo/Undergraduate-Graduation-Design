@echo off

REM 编译所有源文件并生成可执行文件
g++ -o ../bin/peg peg.cpp

:: 检查编译是否成功
if %errorlevel% neq 0 (
    echo 编译失败，请检查源代码。
    pause
    exit /b %errorlevel%
)

REM 运行生成的程序
cd ..
bin\peg.exe
cd -  # 回到之前的目录