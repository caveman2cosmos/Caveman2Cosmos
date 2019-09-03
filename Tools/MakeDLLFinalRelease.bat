@echo off
PUSHD "%~dp0"
call _MakeDLL.bat rebuild FinalRelease stage
POPD
