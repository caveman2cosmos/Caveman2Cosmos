@echo off
PUSHD "%~dp0"
call _MakeDLL.bat Assert rebuild deploy
POPD
