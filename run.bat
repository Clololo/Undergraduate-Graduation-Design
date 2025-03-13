@echo off
gcc -o ./bin/main encoding/encode.c decoding/MinSumBP.c trans/AWGN.c trans/electron_sim.c test.c main.c tools/utils.c tools/mat_tool.c tools/print_tool.c 
if %errorlevel% neq 0 (
    echo Compilation failed.
    exit /b %errorlevel%
)
bin/main.exe
pause
