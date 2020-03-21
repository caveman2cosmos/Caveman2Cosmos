@echo off
echo Launching Caveman2Cosmos ...

PUSHD "%~dp0"

if not exist ..\..\Civ4BeyondSword.exe (
    call :wrong_mods_dir
)

PUSHD "%~dp0..\.."
start "" "Civ4BeyondSword.exe" "mod= mods\Caveman2Cosmos"
POPD
POPD
exit /B 0

:wrong_mods_dir
echo Can't launch Caveman2Cosmos, it is not installed to the correct location!
echo Please install it to Mods\Caveman2Cosmos under your Beyond the Sword install directory.
echo i.e. Not under your My Documents folder or anywhere else.
pause

POPD
exit /B 0
