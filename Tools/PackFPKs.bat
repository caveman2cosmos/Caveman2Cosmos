@echo off

echo.
echo **** README ****
echo This script is part of a suite of scripts for managing the FPK files. They are designed to 
echo make modifying FPK file contents as simple as possible.
echo [Author @billw on discord / @billw2015 on the forums]
echo.
echo This script will:
echo   1. Move your existing SVN workspace FPKs into a temporary directory.
echo   2. Remove the old FPKs
echo   3. Pack all files in the unpacked directory you select into FPKs in the unpacked directory.
echo.
echo.
pause

setlocal ENABLEEXTENSIONS ENABLEDELAYEDEXPANSION
cd /d "%~dp0"
set "ASSETS_DIR=%~dp0..\Assets"
set "FPK_IN_DIR=%~dp0..\UnpackedArt"

echo 1. Moving old FPKs to "%ASSETS_DIR%\oldfpks" ...
mkdir "%ASSETS_DIR%\oldfpks"
move /Y "%ASSETS_DIR%\*.fpk" "%ASSETS_DIR%\oldfpks"

echo 2. Removing old FPKs ...
rmdir /Q /S "%ASSETS_DIR%\oldfpks"

echo 3. Packing FPKs ...
echo From: %FPK_IN_DIR%
echo To:   %ASSETS_DIR%
echo ...
PakBuild /I="%FPK_IN_DIR%" /O="%ASSETS_DIR%" /F /S=100 /R=C2C /X=bik



pause

exit /B 0