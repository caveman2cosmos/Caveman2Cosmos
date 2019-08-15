@echo off
PUSHD "%~dp0"
call _MakeDLL.bat rebuild Release
POPD
