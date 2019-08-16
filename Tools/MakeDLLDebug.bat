@echo off
PUSHD "%~dp0"
call _MakeDLL.bat rebuild Debug
POPD
echo error code: %errorlevel%