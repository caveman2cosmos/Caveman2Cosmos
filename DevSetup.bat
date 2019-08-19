@echo off

echo.
echo **** README ****
echo.
echo This script will do default dev setup/update including a LINK based install, and building 
echo the Assert DLL. Packing FPKs will be done automatically when the mod is run.
echo.
echo [Author @billw on discord / @billw2015 on the forums]
echo.

pause

PUSHD "%~dp0"
call Tools\Install.bat
call Tools\MakeDLLAssert.bat
POPD

echo.
echo.
echo.
echo **** Dev Setup complete, you can run the Mod now! ****
pause
