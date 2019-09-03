@echo off

REM Make sure the dependencies are extracted
PUSHD "%~dp0"
call InstallDeps.bat
POPD

REM Set the environment paths so we can find nmake
PUSHD "%~dp0..\Build\deps\Microsoft Visual C++ Toolkit 2003"
SET PATH=%cd%\bin;%PATH%
POPD

REM Switch to the source directory
PUSHD "%~dp0..\Sources

set TARGET=%2

if "%1"=="build" (
    echo Building DLL in %2 configuration ...
    nmake source_list
    nmake fastdep
    nmake precompile
    ..\Build\deps\jom\jom build
    if "%3"=="stage" (
        nmake stage
    )
)
if "%1"=="rebuild" (
    echo Rebuilding DLL in %2 configuration ...
    nmake clean
    nmake source_list
    nmake fastdep
    nmake precompile
    ..\Build\deps\jom\jom build
    if "%3"=="stage" (
        nmake stage
    )
)
if "%1"=="clean" (
    echo Cleaning DLL in %2 configuration ...
    nmake clean
)

REM Restore original directory
POPD

echo ...Finished
