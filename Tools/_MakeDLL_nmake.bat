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

set ACTION=%1
set TARGET=%2

:retry
if "%ACTION%"=="build" (
    echo Building DLL in %TARGET% configuration ...
    call :run_cmd_checked nmake source_list
    if errorlevel 1 goto :exit_failed
    call :run_cmd_checked nmake fastdep
    if errorlevel 1 goto :exit_failed
    call :run_cmd_checked nmake precompile
    if errorlevel 1 goto :exit_failed
    call :run_cmd_checked ..\Build\deps\jom\jom build
    if errorlevel 1 goto :exit_failed
    call :run_cmd_checked nmake stage
    if errorlevel 1 goto :exit_failed
)
if "%ACTION%"=="rebuild" (
    echo Rebuilding DLL in %TARGET% configuration ...
    call :run_cmd_checked nmake clean
    if errorlevel 1 goto :exit_failed
    call :run_cmd_checked nmake source_list
    if errorlevel 1 goto :exit_failed
    call :run_cmd_checked nmake fastdep
    if errorlevel 1 goto :exit_failed
    call :run_cmd_checked nmake precompile
    if errorlevel 1 goto :exit_failed
    call :run_cmd_checked ..\Build\deps\jom\jom build
    if errorlevel 1 goto :exit_failed
    call :run_cmd_checked nmake stage
    if errorlevel 1 goto :exit_failed
)
if "%ACTION%"=="autobuild" (
    call :autobuild
    if errorlevel 1 (
        set ACTION=rebuild
        goto :retry
    )
)
if "%ACTION%"=="clean" (
    echo Cleaning DLL in %TARGET% configuration ...
    call :run_cmd_checked nmake clean
    if errorlevel 1 goto :exit_failed
)

REM Restore original directory
POPD

:exit_okay
echo ...Finished
exit /B 0

:exit_failed
echo ...Failed
exit /B 1

:autobuild
echo Autobuilding DLL in %TARGET% configuration ...
nmake source_list
if errorlevel 1 (
    echo Failed to build source list, falling back to full rebuild...
    exit /B 1
)
nmake fastdep
if errorlevel 1 (
    echo Failed to run fastdep, falling back to full rebuild...
    exit /B 1
)
nmake precompile
if errorlevel 1 (
    echo Failed to do precompile, falling back to full rebuild...
    exit /B 1
)
..\Build\deps\jom\jom build
if errorlevel 1 (
    echo Failed to do autobuild, falling back to full rebuild...
    exit /B 1
)
exit /B 0

:run_cmd_checked
%*
if errorlevel 1 (
    echo Command %1 failed with error code %errorlevel%
    exit /B 1
)
exit /B 0