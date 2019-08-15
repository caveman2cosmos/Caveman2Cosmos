@echo off

REM Make sure the dependencies are extracted
echo Checking dependencies...
if not exist ..\sources\deps (
    echo Dependencies not found! 
    echo Unpacking Caveman2Cosmos DLL dependencies, please wait for the process to complete...
    start /b /wait deps.exe -y -o..\sources
)
echo ...Done!

POPD
