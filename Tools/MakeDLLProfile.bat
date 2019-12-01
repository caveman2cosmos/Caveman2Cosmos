@echo off
PUSHD "%~dp0"
call _MakeDLL.bat Profile rebuild deploy
POPD
