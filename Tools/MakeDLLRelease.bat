@echo off
PUSHD "%~dp0"
call _MakeDLL.bat rebuild Release
POPD
REM Force clean exit code
exit /B 0
