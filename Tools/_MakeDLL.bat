@echo off

REM Make sure the dependencies are extracted
PUSHD "%~dp0"
set "tools_dir=%cd%"
call InstallDeps.bat
POPD

REM Switch to the source directory
PUSHD "%tools_dir%\..\Sources"

if "%APPVEYOR_REPO_BRANCH%" eq "%release_branch%" (
    echo Skipping test build due to being on release branch
    exit /b 0
)

call Powershell.exe -NoProfile -ExecutionPolicy Bypass -File "%tools_dir%\_Build.ps1" %*
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
