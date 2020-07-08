::::::::::::::::::::::::::::::::::::::::::::
:: Elevate.cmd - Version 4
:: Automatically check & get admin rights
:: see "https://stackoverflow.com/a/12264592/1016343" for description
::::::::::::::::::::::::::::::::::::::::::::
 @echo off
 CLS
 ECHO.
 ECHO =============================
 ECHO Running Admin shell
 ECHO =============================

:init
 setlocal DisableDelayedExpansion
 set cmdInvoke=1
 set winSysFolder=System32
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

  if '%cmdInvoke%'=='1' goto InvokeCmd 

  ECHO UAC.ShellExecute "!batchPath!", args, "", "runas", 1 >> "%vbsGetPrivileges%"
  goto ExecElevation

:InvokeCmd
  ECHO args = "/c """ + "!batchPath!" + """ " + args >> "%vbsGetPrivileges%"
  ECHO UAC.ShellExecute "%SystemRoot%\%winSysFolder%\cmd.exe", args, "", "runas", 1 >> "%vbsGetPrivileges%"

:ExecElevation
 "%SystemRoot%\%winSysFolder%\WScript.exe" "%vbsGetPrivileges%" %*
 exit /B

:gotPrivileges
 setlocal & cd /d %~dp0
 if '%1'=='ELEV' (del "%vbsGetPrivileges%" 1>nul 2>nul  &  shift /1)

 ::::::::::::::::::::::::::::
 ::START
 ::::::::::::::::::::::::::::

@echo off

set build_config=Release

echo.
echo **** README ****
echo.
echo This script will do default dev setup/update including a LINK based
echo install, building the %build_config% DLL.
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
if errorlevel 1 (
    call :install_failure
    exit /B 1
)
call "%~dp0\Tools\_MakeDLL.bat" %build_config% build deploy
if errorlevel 1 (
    call :build_failure
    exit /B 1
)
call "%~dp0\Tools\UpdateVSUserFile.bat"
POPD

echo.
echo.
echo.
echo **** Dev Setup complete, you can run the Mod now using LaunchC2C.bat or your prefered method! ****
pause

exit /B 0

:install_failure
echo Install failed, please check the log above this message for warnings or errors!
echo If you need help please ask in the forums or discord.
pause

exit /B 0

:build_failure
echo DLL compile failed, please check the log above this message for warnings or errors!
echo If you need help please ask in the forums or discord.
pause

exit /B 0
