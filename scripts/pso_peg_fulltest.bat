# ��Ҫ��ǰִ��һ��psotest.bat

REM �������ɵĳ���
cd ..
bin\PSOtest.exe
cd -  # �ص�֮ǰ��Ŀ¼
cd script

REM ��������Դ�ļ������ɿ�ִ���ļ�
g++ -o ../bin/peg ../src/PSO_OPT/peg.cpp

:: �������Ƿ�ɹ�
if %errorlevel% neq 0 (
    echo ����ʧ�ܣ�����Դ���롣
    pause
    exit /b %errorlevel%
)

REM �������ɵĳ���
cd ..
bin\peg.exe
cd -  # �ص�֮ǰ��Ŀ¼