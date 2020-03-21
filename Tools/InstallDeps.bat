@echo off

PUSHD "%~dp0"

:: Make sure the dependencies are extracted
echo Checking dependencies...
:: We use a text file in the deps folder with the version in the name to decide if deps are up to date
if exist "..\Build\deps" (
    if not exist "..\Build\deps\deps.v4.txt" (
        echo Dependencies outdated, updating to v4!
        echo Deleting Caveman2Cosmos DLL dependencies, please wait for the process to complete...
        rmdir /Q /S "..\Build\deps"

        echo Unpacking Caveman2Cosmos DLL dependencies, please wait for the process to complete...
        start /b /wait deps.exe -y -o..\Build
    )
) else (
    echo Dependencies not found! 
    echo Unpacking Caveman2Cosmos DLL dependencies, please wait for the process to complete...
    start /b /wait deps.exe -y -o..\Build
)
echo ...Done!

POPD
