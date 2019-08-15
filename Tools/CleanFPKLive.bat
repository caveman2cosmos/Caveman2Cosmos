@echo off
echo.
echo **** README ****
echo This script will delete the FPK Live token file, causing a full FPK rebuild next time 
echo you run C2C.
echo [Author @billw on discord / @billw2015 on the forums]
echo.
pause
del "%~dp0..\Assets\fpklive_token.txt"
