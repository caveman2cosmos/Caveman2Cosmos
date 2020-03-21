@echo off
PUSHD "%~dp0"
call _MakeDLL.bat Debug rebuild deploy
POPD
REM Force clean exit code
exit /B 0
