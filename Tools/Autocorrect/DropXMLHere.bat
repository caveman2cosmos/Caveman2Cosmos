@echo off

PUSHD "%~dp0"

set PYTHON="%~dp0..\python\tools\python.exe"

if not exist "%PYTHON%" (
    echo Python install not found!
    echo Please make sure to run StartServer.bat to setup python and run the LanguageTool server!
    pause
    exit /B 1
)
%PYTHON% Autocorrect.py --interactive %*

POPD
