@echo off

REM ��������Դ�ļ������ɿ�ִ���ļ�
g++ -o peg.exe peg.cpp

:: �������Ƿ�ɹ�
if %errorlevel% neq 0 (
    echo ����ʧ�ܣ�����Դ���롣
    pause
    exit /b %errorlevel%
)

REM �������ɵĳ���
peg.exe