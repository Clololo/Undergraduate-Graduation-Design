@echo off
gcc -o ../bin/LDPCrun ^
       ../src/encoding/encode.c ../src/decoding/pso.c ../src/decoding/MinSumBP.c ^
       ../src/trans/AWGN.c ../src/trans/electron_sim.c ../src/LDPC.c ../src/test.c ^
       ../src/tools/utils.c ../src/tools/mat_tool.c ../src/tools/print_tool.c ../src/tools/math_tool.c 
       
if %errorlevel% neq 0 (
    echo Compilation failed.
    exit /b %errorlevel%
)
cd ..
bin\LDPCrun.exe
cd .\scripts
pause
