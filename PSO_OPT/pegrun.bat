@echo off

REM ��������Դ�ļ������ɿ�ִ���ļ�
g++ -o ../bin/peg peg.cpp

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