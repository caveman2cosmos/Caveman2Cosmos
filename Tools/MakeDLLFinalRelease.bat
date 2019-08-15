@echo off
PUSHD "%~dp0"
call _MakeDLL.bat rebuild FinalRelease
POPD
