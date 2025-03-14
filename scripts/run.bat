@echo off
gcc -o ../bin/main ../src/encoding/encode.c ../src/decoding/MinSumBP.c ../src/trans/AWGN.c ../src/trans/electron_sim.c ../src/test.c ../src/main.c ../src/tools/utils.c ../src/tools/mat_tool.c ../src/tools/print_tool.c 
if %errorlevel% neq 0 (
    echo Compilation failed.
    exit /b %errorlevel%
)
cd ..
bin/main.exe
cd -
pause
