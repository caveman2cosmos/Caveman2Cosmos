@echo off

PUSHD "%~dp0..\Sources
"%~dp0fbuild.exe" -cachetrim 512
POPD
