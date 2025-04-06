@echo off
gcc -o ../bin/trainrun ^
       ../src/encoding/encode.c ../src/decoding/pso.c  ^
       ../src/trans/AWGN.c ../src/trans/electron_sim.c  ../src/train.c ^
       ../src/tools/utils.c ../src/tools/mat_tool.c ../src/tools/print_tool.c ../src/tools/math_tool.c ^
       ../src/trans/trans.c  ../src/decoding/NMS_WBP.c ../src/newencoding/bg2read.c ../src/newencoding/encode.c 
    
if %errorlevel% neq 0 (
    echo Compilation failed.
    exit /b %errorlevel%
)
cd ..
bin\trainrun.exe
cd .\scripts
pause
