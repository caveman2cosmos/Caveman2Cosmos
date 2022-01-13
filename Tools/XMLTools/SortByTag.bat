@echo off

PUSHD "%~dp0..\.."

set PYTHON="%~dp0..\python\tools\python.exe"

if not exist "%PYTHON%" (
    echo Python install not found, installing now...
    call Tools\Setup\InstallPython3.bat!
)
%PYTHON% Tools\XMLTools\SortByTag.py %*

POPD

PAUSE