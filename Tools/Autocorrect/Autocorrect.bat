@echo off
PUSHD "%~dp0..\.."
python Tools\Autocorrect\Autocorrect.py %*
POPD