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

set TARGET=%1-build
set FBUILD_ARGS=-summary -wait -wrapper -cache -cacheverbose
set DEPLOY_DIR="%~dp0..\Assets"
set FBUILD="%~dp0fbuild.exe"

if "%2" NEQ "" (
    call :do_action %1 %2
)
if errorlevel 1 (
    echo Action %2 on %1 failed with error code %errorlevel%
    goto :exit_failed
)

if "%3" NEQ "" (
    call :do_action %1 %3
)
if errorlevel 1 (
    echo Action %3 on %1 failed with error code %errorlevel%
    goto :exit_failed
)

if "%4" NEQ "" (
    call :do_action %1 %4
)
if errorlevel 1 (
    echo Action %4 on %1 failed with error code %errorlevel%
    goto :exit_failed
)

REM Restore original directory
POPD

goto :exit_okay

:do_action
set TARGET_DIR="%~dp0..\Build\%1"
set ACTION=%2
if "%ACTION%"=="build" (
    echo Building DLL in %TARGET% configuration ...
    call :run_cmd_checked %FBUILD% %FBUILD_ARGS% %TARGET%
)
if "%ACTION%"=="rebuild" (
    echo Rebuilding DLL in %TARGET% configuration ...
    call :run_cmd_checked %FBUILD% %FBUILD_ARGS% -clean %TARGET%
)
if "%ACTION%"=="deploy" (
    echo Deploying DLL in %TARGET% configuration ...
    call :deploy %TARGET_DIR% %DEPLOY_DIR%
)
if "%ACTION%"=="clean" (
    echo Cleaning target %TARGET% configuration ...
    call :clean %TARGET_DIR%
)
if errorlevel 1 (
    exit /B 1
)
exit /B 0

:exit_okay
echo ...Finished
exit /B 0

:exit_failed
echo ...Failed
exit /B 1

:run_cmd_checked
%*
if errorlevel 1 (
    echo Command %1 failed with error code %errorlevel%
    exit /B 1
)
exit /B 0

:deploy
call :run_cmd_checked xcopy "%~1\CvGameCoreDLL.dll" "%~2" /R /Y
if errorlevel 1 (
    exit /B 1
)
call :run_cmd_checked xcopy "%~1\CvGameCoreDLL.pdb" "%~2" /R /Y
if errorlevel 1 (
    exit /B 1
)
exit /B 0

:clean
call :run_cmd_checked rmdir /S /Q "%~1"
if errorlevel 1 (
    exit /B 1
)
exit /B 0