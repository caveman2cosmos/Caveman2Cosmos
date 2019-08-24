@echo off

:: Make sure the dependencies are extracted
echo Checking dependencies...
:: TODO: use a token file and git SHA or deps.exe stats (size/hash/etc) to decide if deps need (re)installing
if not exist "..\sources\deps\Microsoft Visual C++ Toolkit 2003\bin\nmake.exe" (
    echo Dependencies not found! 
    echo Unpacking Caveman2Cosmos DLL dependencies, please wait for the process to complete...
    start /b /wait deps.exe -y -o..\sources
)
echo ...Done!

POPD
