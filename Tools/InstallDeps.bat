@echo off

PUSHD "%~dp0"

:: Make sure the dependencies are extracted
echo Checking dependencies...
:: TODO: use a token file and git SHA or deps.exe stats (size/hash/etc) to decide if deps need (re)installing
if exist "..\Build\deps\Microsoft Platform SDK" (
	echo Dependencies outdated!
	echo Deleting Caveman2Cosmos DLL dependencies, please wait for the process to complete...
	rmdir /Q /S "..\Build\deps"
)
if not exist "..\Build\deps\Microsoft Visual C++ Toolkit 2003\bin\nmake.exe" (
    echo Dependencies not found! 
    echo Unpacking Caveman2Cosmos DLL dependencies, please wait for the process to complete...
    start /b /wait deps.exe -y -o..\Build
)
echo ...Done!

POPD
