@echo off
PUSHD "%~dp0"
call _MakeDLL.bat FinalRelease rebuild deploy
POPD
