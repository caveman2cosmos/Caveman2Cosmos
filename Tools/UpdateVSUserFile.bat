
@echo off

PUSHD "%~dp0"

:: Install dir not set, ask the user for it
if not exist mods_directory.txt (
    GetDirectory "Please select your Beyond the Sword Mods directory" >mods_directory.txt
) else (
    echo Using Mods directory saved from previous run of this script ...
)

set /p MODS_DIR=<mods_directory.txt

PUSHD "%MODS_DIR%\.."
set "BTS_DIR=%cd%"
POPD

powershell -Command "(gc '..\Sources\C2C (VS2019).vcxproj.user.template') -replace '{BTSDIR}', '%BTS_DIR%' | Out-File -encoding ASCII '..\Sources\C2C (VS2019).vcxproj.user'"

POPD
