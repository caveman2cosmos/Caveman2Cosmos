@echo off
PUSHD "%~dp0"
call _MakeDLL.bat ProfileExtra rebuild deploy
POPD
