@echo off
setlocal enabledelayedexpansion

:: 记录开始时间
set "start_time=%time%"

REM 编译所有源文件并生成可执行文件
gcc -o ../bin/PSOtest ../src/PSO_OPT/psotest.c ../src/PSO_OPT/pso.c ../src/PSO_OPT/FDE.c ../src/PSO_OPT/GA.c ../src/tools/print_tool.c ../src/tools/math_tools.c

REM 检查是否编译成功
if %errorlevel% neq 0 (
    echo Compilation failed.
    exit /b %errorlevel%
)

REM 编译所有源文件并生成可执行文件
g++ -o ../bin/peg ../src/PSO_OPT/peg.cpp ../src/tools/print_tool.c ../src/tools/math_tools.c

:: 检查编译是否成功
if %errorlevel% neq 0 (
    echo 编译失败，请检查源代码。
    pause
    exit /b %errorlevel%
)

REM 记录编译完成时间
set "compile_end_time=%time%"

REM 运行生成的程序
cd ..
set "exec_start_time=%time%"

bin\PSOtest.exe
bin\peg.exe

set "exec_end_time=%time%"

cd scripts

:: 计算总耗时
call :get_time_difference "%compile_end_time%" "%exec_end_time%" "运行耗时"

exit /b

:: 计算时间差的函数
:get_time_difference
setlocal
for /f "tokens=1-4 delims=:." %%a in (%1) do (
    set /a "start_h=%%a, start_m=%%b, start_s=%%c, start_ms=%%d"
)
for /f "tokens=1-4 delims=:." %%a in (%2) do (
    set /a "end_h=%%a, end_m=%%b, end_s=%%c, end_ms=%%d"
)

:: 将时间转换为毫秒
set /a "start_total_ms=(start_h*3600 + start_m*60 + start_s)*100 + start_ms"
set /a "end_total_ms=(end_h*3600 + end_m*60 + end_s)*100 + end_ms"

:: 处理跨午夜情况
if %end_total_ms% LSS %start_total_ms% (
    set /a "end_total_ms+=8640000"
)

:: 计算耗时
set /a "diff_ms=end_total_ms-start_total_ms"
set /a "diff_h=diff_ms/360000"
set /a "diff_m=(diff_ms%%360000)/6000"
set /a "diff_s=(diff_ms%%6000)/100"
set /a "diff_ms=diff_ms%%100"

:: 输出结果
echo %3: %diff_h% 小时 %diff_m% 分钟 %diff_s% 秒 %diff_ms% 毫秒
endlocal
exit /b
