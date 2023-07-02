::::::::::::::::::::::::::::::::::::::::::::
:: Automatically check & get admin rights V2
::::::::::::::::::::::::::::::::::::::::::::
@echo off

if "%1"=="DevSetup" goto :skip_readme

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
echo This script will link from your Git workspace directly into your Mods folder.
echo It is designed so you can have your Git workspace OUTSIDE of the BtS Mods folder
echo but still run the Mod without having to export or copy. It does this by making links
echo from specific directories in the Git workspace directly into the Mods directory.
echo Once this script is run you do NOT need to run it again unless you do a clean
echo fetch from Git or otherwise delete/move the folders in the workspace.
echo -- [Author @billw on discord / @billw2015 on the forums]
echo.
echo This script will:
echo   1. (optional) Give you a directory browser to select your Mods location.
echo   2. Create the Caveman2Cosmos directory under Mods.
echo   3. Create the appropriate links from your Git workspace to that new directory.
echo   4. Copy any files that shouldn't be linked to that new directory.
echo.
echo After it is complete you should be able to run the Mod without issue.
echo.
echo IMPORTANT:
echo     Running this script is only appropriate if your Git workspace (from which you are 
echo     running now) is OUTSIDE the Mods directory. If you want to have Git in the Mods 
echo     directory then this script isn't for you.
echo NOTE:
echo     If you previously used this script then it should have saved the Mods directory 
echo     from last time. If you want to reset the saved directory then delete 
echo     Tools\mods_directory.txt
echo.
pause

:skip_readme

PUSHD "%~dp0"

setlocal

if exist "..\..\..\Civ4BeyondSword.exe" (
    echo.
    echo *** ERROR ***
    echo.
    echo It looks like you are running this script from within the BtS Mods directory.
    echo As explained in the README above, it is intended to install TO the Mods directory FROM
    echo somewhere else!
    echo If you want to use it then move your Git workspace out of the Mods directory
    echo to somewhere else, then run this script again from there instead.
    pause
    exit /B 1
)

echo 1. Locate Mods directory ...

:: Install dir not set, ask the user for it
if not exist mods_directory.txt (
    GetDirectory "Please select your Beyond the Sword Mods directory" >mods_directory.txt
) else (
    echo Using Mods directory saved from previous run of this script ...
)

:: Read in the saved mods directory and check it still exists
set /p MODS_DIR=<mods_directory.txt
if not exist "%MODS_DIR%" (
    del mods_directory.txt
    GetDirectory "Please select your Beyond the Sword Mods directory" >mods_directory.txt
)

if not exist "%MODS_DIR%\..\Civ4BeyondSword.exe" (
    del mods_directory.txt
    call :wrong_mods_dir "!MODS_DIR!"
    exit /B 1
)

:: If the C2C directory already exists then we can offer to delete it or abort
set "C2C_MOD_DIR=%MODS_DIR%\Caveman2Cosmos"
if exist "%C2C_MOD_DIR%" (
    :: We will only warn if it isn't an install done previously with this tool
    if not exist "!C2C_MOD_DIR!\git_directory.txt" (
        call :warn_dir_exists "!C2C_MOD_DIR!"
        if %errorlevel% neq 0 exit /B 1
    ) else (
        rmdir /S /Q "!C2C_MOD_DIR!"
    )
)

echo 2. Create Caveman2Cosmos directory ...
mkdir "%C2C_MOD_DIR%"

PUSHD ..

:: Make the junctions and copy the files we don't want to modify in Git
echo 3. Making required links from '%cd%' Git directory to '%C2C_MOD_DIR%' ...
mklink /J "%C2C_MOD_DIR%\Assets" Assets
mklink /J "%C2C_MOD_DIR%\PrivateMaps" PrivateMaps
mklink /J "%C2C_MOD_DIR%\PublicMaps" PublicMaps
mklink /J "%C2C_MOD_DIR%\Resource" Resource

echo 4. Copying config file ...
copy "Caveman2Cosmos.ini" "%C2C_MOD_DIR%"

:: Write a back pointer so we know where the Git repo is
echo %cd%>"%C2C_MOD_DIR%\git_directory.txt"

POPD
POPD

endlocal

echo Done!

if "%1"=="DevSetup" goto :skip_end_pause
pause

:skip_end_pause
exit /B 0

:: rmdir /S/Q "%MOD_DIR%"
:: mkdir "%MOD_DIR%"

:: call PackFPKs.bat
:: call MakeDLLRelease.bat
:: call Copy.bat

:warn_dir_exists

for /f "tokens=2-8 delims=.:/ " %%a in ("%date% %time%") do set DateNtime=%%c-%%a-%%b_%%d-%%e-%%f.%%g
set new_name=Caveman2Cosmos_%DateNtime%
echo.
echo *** WARNING ***
echo.
echo Caveman2Cosmos already exists in the mods folder %1.
echo It does not appear to be there from a previous install with this script.
echo This operation will RENAME it to %new_name% and create a new mod directory
echo from scratch, are you sure?
echo To cancel close this window, or press Ctrl+C
echo or
pause

PUSHD "%~1\.."
MOVE /-Y "%~1" "%new_name%""
if %errorlevel% neq 0 (
    echo Something went wrong backing up your old Caveman2Cosmos folder, please
    echo close this Window and renamem, move or delete the folder yourself
    pause
    exit /B 1
)
POPD

exit /B 0

:wrong_mods_dir
echo.
echo *** ERROR ***
echo.
echo %1 does NOT appear to be your Mods directory. 
echo I looked for Civ4BeyondSword.exe in a parent directory and couldn't find it!
echo If you believe this check is incorrect then either edit the Install script
echo to remove it, or contact the author and explain why!
echo.
echo This script will terminate, please run it again to select a different directory!
pause

exit /B 0