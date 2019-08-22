::::::::::::::::::::::::::::::::::::::::::::
:: Automatically check & get admin rights V2
::::::::::::::::::::::::::::::::::::::::::::
@echo off
CLS

:init
setlocal DisableDelayedExpansion
set "batchPath=%~0"
for %%k in (%0) do set batchName=%%~nk
set "vbsGetPrivileges=%temp%\OEgetPriv_%batchName%.vbs"
setlocal EnableDelayedExpansion

:checkPrivileges
NET FILE 1>NUL 2>NUL
if '%errorlevel%' == '0' ( goto gotPrivileges ) else ( goto getPrivileges )

:getPrivileges
if '%1'=='ELEV' (echo ELEV & shift /1 & goto gotPrivileges)
ECHO.
ECHO **************************************
ECHO Invoking UAC for Privilege Escalation
ECHO **************************************

ECHO Set UAC = CreateObject^("Shell.Application"^) > "%vbsGetPrivileges%"
ECHO args = "ELEV " >> "%vbsGetPrivileges%"
ECHO For Each strArg in WScript.Arguments >> "%vbsGetPrivileges%"
ECHO args = args ^& strArg ^& " "  >> "%vbsGetPrivileges%"
ECHO Next >> "%vbsGetPrivileges%"
ECHO UAC.ShellExecute "!batchPath!", args, "", "runas", 1 >> "%vbsGetPrivileges%"
"%SystemRoot%\System32\WScript.exe" "%vbsGetPrivileges%" %*
exit /B

:gotPrivileges
setlocal & pushd .
cd /d %~dp0
if '%1'=='ELEV' (del "%vbsGetPrivileges%" 1>nul 2>nul  &  shift /1)

::::::::::::::::::::::::::::
::START
::::::::::::::::::::::::::::

@echo off

echo.
echo **** README ****
echo.
echo This script will do default dev setup/update including a LINK based
echo install, and building the Assert DLL.
echo Packing FPKs will be done automatically when the mod is run.
echo. 
echo *** WARNINGS - MAKE SURE YOU READ THESE *** 
echo     If you have Caveman2Cosmos in your Mods directory already (that was not
echo     created by this script) then you should move it or back it up NOW.
echo     This script will replace it entirely.
echo     It will attempt to rename any existing directory rather than delete it,
echo     but if you have changes in there you want to keep then the safest bet
echo     is to do it yourself BEFORE continuing.
echo.
echo     Once you have installed using this script you should ONLY make changes
echo     in your Git directory, NOT in the Mods\Caveman2Cosmos directory. 
echo     Changes done in the Mods\Caveman2Cosmos directory may be lost when you
echo     run this script again.
echo.
echo [Author @billw on discord / @billw2015 on the forums]
echo.

pause

PUSHD "."
call "%~dp0\Tools\Setup\InstallGit.bat"
call "%~dp0\Tools\Install.bat" DevSetup
call "%~dp0\Tools\_MakeDLL.bat" build Assert
POPD

echo.
echo.
echo.
echo **** Dev Setup complete, you can run the Mod now using LaunchC2C.bat or your prefered method! ****
pause
