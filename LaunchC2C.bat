@echo off
echo Launching Caveman2Cosmos ...

PUSHD "%~dp0"

if exist ..\..\Civ4BeyondSword.exe (
    call :run_from_mods_dir
) else (
    call :run_from_git_dir
)

POPD

exit /B 0

:run_from_git_dir

:: Install dir not set, ask the user for it
if not exist Tools\mods_directory.txt (
    call :not_installed
    exit /B 1
)
echo Found saved Mods directory ...

:: Read in the saved mods directory and check it still exists
set /p MODS_DIR=<Tools\mods_directory.txt
if not exist "%MODS_DIR%" (
    call :no_mods_dir
    exit /B 2
)

if not exist "%MODS_DIR%\..\Civ4BeyondSword.exe" (
    call :wrong_mods_dir
    exit /B 3
)

if not exist "%MODS_DIR%\Caveman2Cosmos\git_directory.txt" (
    call :git_mod_not_active
    exit /B 4
)

PUSHD "%MODS_DIR%\.."
start "" "Civ4BeyondSword.exe" "mod= mods\Caveman2Cosmos"
POPD

exit /B 0

:run_from_mods_dir
PUSHD "%~dp0..\.."
start "" "Civ4BeyondSword.exe" "mod= mods\Caveman2Cosmos"
POPD
exit /B 0

:not_installed
echo.
echo.
echo ERROR: It looks like you have not run DevSetup.bat.
echo Please run DevSetup.bat before trying to use this script to start Caveman2Cosmos.
pause
exit /B 0

:no_mods_dir
del mods_directory.txt
echo.
echo.
echo ERROR: Saved mods directory doesn't exist!
echo Please rerun DevSetup.bat script to select a new one.
pause
exit /B 0

:wrong_mods_dir
del mods_directory.txt
echo.
echo.
echo ERROR: Saved mods directory doesn't appear to be inside the BtS install location.
echo Please rerun DevSetup.bat script to select the Mods directory inside your BtS installation (NOT in My Documents).
pause
exit /B 0

:git_mod_not_active
echo.
echo.
echo ERROR: This isn't the currently active version of the mod. 
echo Did you rename your Mods\Caveman2Cosmos dir so you could switch to a different version?
pause
exit /B 0
