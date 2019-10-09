@echo off
PUSHD "%~dp0..\..\UnpackedArt"

python "%~dp0.\c2c-toaster.py" dump_default_lights art | find "NiDirectionalLight in " > ..\lights-found.txt

POPD
