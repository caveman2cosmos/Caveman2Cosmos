@echo off
PUSHD "%~dp0"
call _MakeDLL.bat rebuild Debug
POPD
REM Force clean exit code
exit /B 0
