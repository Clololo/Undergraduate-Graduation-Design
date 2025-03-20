@echo off
gcc -o ../bin/gen ^
        ../src/encoding/encode.c ../src/decoding/MinSumBP.c ../src/trans/AWGN.c ../src/trans/electron_sim.c ^
        ../src/generate_H.c ../src/tools/utils.c ../src/tools/mat_tool.c ../src/tools/print_tool.c 
if %errorlevel% neq 0 (
    echo Compilation failed.
    exit /b %errorlevel%
)
cd ..
bin/peg.exe
cd -
pause
