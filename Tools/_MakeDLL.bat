@echo off

REM Make sure the dependencies are extracted
PUSHD "%~dp0"
call InstallDeps.bat
POPD

REM Switch to the source directory
PUSHD "%~dp0..\Sources"

call Powershell.exe -NoProfile -ExecutionPolicy Bypass -File "%~dp0_Build.ps1" %*
if %ERRORLEVEL% NEQ 0 (
    goto :exit_failed
)

REM Restore original directory
POPD

echo ...Finished
exit /B 0

:exit_failed
echo ...Failed
exit /B 1
