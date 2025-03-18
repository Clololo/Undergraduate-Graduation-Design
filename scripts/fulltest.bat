@echo off
setlocal enabledelayedexpansion

:: ��¼��ʼʱ��
set "start_time=%time%"

REM ��������Դ�ļ������ɿ�ִ���ļ�
gcc -o ../bin/PSOtest ../src/PSO_OPT/psotest.c ../src/PSO_OPT/pso.c ../src/PSO_OPT/FDE.c ../src/PSO_OPT/GA.c ../src/tools/print_tool.c ../src/tools/math_tools.c

REM ����Ƿ����ɹ�
if %errorlevel% neq 0 (
    echo Compilation failed.
    exit /b %errorlevel%
)

REM ��������Դ�ļ������ɿ�ִ���ļ�
g++ -o ../bin/peg ../src/PSO_OPT/peg.cpp ../src/tools/print_tool.c ../src/tools/math_tools.c

:: �������Ƿ�ɹ�
if %errorlevel% neq 0 (
    echo ����ʧ�ܣ�����Դ���롣
    pause
    exit /b %errorlevel%
)

REM ��¼�������ʱ��
set "compile_end_time=%time%"

REM �������ɵĳ���
cd ..
set "exec_start_time=%time%"

bin\PSOtest.exe
bin\peg.exe

set "exec_end_time=%time%"

cd scripts

:: �����ܺ�ʱ
call :get_time_difference "%compile_end_time%" "%exec_end_time%" "���к�ʱ"

exit /b

:: ����ʱ���ĺ���
:get_time_difference
setlocal
for /f "tokens=1-4 delims=:." %%a in (%1) do (
    set /a "start_h=%%a, start_m=%%b, start_s=%%c, start_ms=%%d"
)
for /f "tokens=1-4 delims=:." %%a in (%2) do (
    set /a "end_h=%%a, end_m=%%b, end_s=%%c, end_ms=%%d"
)

:: ��ʱ��ת��Ϊ����
set /a "start_total_ms=(start_h*3600 + start_m*60 + start_s)*100 + start_ms"
set /a "end_total_ms=(end_h*3600 + end_m*60 + end_s)*100 + end_ms"

:: �������ҹ���
if %end_total_ms% LSS %start_total_ms% (
    set /a "end_total_ms+=8640000"
)

:: �����ʱ
set /a "diff_ms=end_total_ms-start_total_ms"
set /a "diff_h=diff_ms/360000"
set /a "diff_m=(diff_ms%%360000)/6000"
set /a "diff_s=(diff_ms%%6000)/100"
set /a "diff_ms=diff_ms%%100"

:: ������
echo %3: %diff_h% Сʱ %diff_m% ���� %diff_s% �� %diff_ms% ����
endlocal
exit /b
