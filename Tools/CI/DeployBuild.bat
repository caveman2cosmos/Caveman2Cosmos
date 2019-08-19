@echo off
PUSHD "%~dp0..\.."

if "%APPVEYOR_REPO_BRANCH%" neq "release" (
    echo Skipping deployment due to not being on release branch
    exit /b 0
)

if "%APPVEYOR_PULL_REQUEST_TITLE%" neq "" (
    echo Skipping deployment due to being a PR build
    exit /b 0
)

echo C2C %APPVEYOR_BUILD_VERSION% DEPLOYMENT
echo.

echo Packing FPKs...
call Tools\PackFPKs.bat CI
if %ERRORLEVEL% neq 0 (
    echo Packing FPKs failed, aborting deployment
    exit /B 1
)

echo Building FinalRelease DLL...
echo TEMPORARILY DISABLED FOR APPVEYOR TESTING!
REM call Tools\MakeDLLFinalRelease.bat
REM if not errorlevel 0 (
REM     echo Building FinalRelease DLL failed, aborting deployment!
REM     exit /B 2
REM )

call Tools\CI\DoSourceIndexing.bat

cd /d "%~dp0..\.."
if not exist "%build_dir%" goto :checkout
rmdir /Q /S "%build_dir%"

:checkout
echo Checking out SVN working copy for deployment...
svn checkout %svn_url% "%build_dir%"

:: HERE IS WHERE YOU ADJUST WHAT TO PUT IN THE BUILD
echo Updating SVN working copy from git...
set ROBOCOPY_FLAGS=/MIR /NFL /NDL /NJH /NJS /NS /NC
robocopy Assets "%build_dir%\Assets" %ROBOCOPY_FLAGS%
robocopy PrivateMaps "%build_dir%\PrivateMaps" %ROBOCOPY_FLAGS%
robocopy Resource "%build_dir%\Resource" %ROBOCOPY_FLAGS%
xcopy Caveman2Cosmos.ini "%build_dir%" /R /Y
xcopy "Caveman2Cosmos Config.ini" "%build_dir%" /R /Y

echo Detecting working copy changes...
PUSHD "%build_dir%"
set SVN=svn.exe
"%SVN%" status | findstr /R "^!" > ..\missing.list
for /F "tokens=* delims=! " %%A in (..\missing.list) do (svn delete "%%A")
del ..\missing.list 2>NUL
"%SVN%" add * --force

REM svn status | findstr /R "^?" > ..\added.list
REM for /F "tokens=* delims=? " %%A in (..\added.list) do (svn add "%%A")
REM del ..\added.list 2>NUL

REM for /f "tokens=2*" %%i in ('svn status "%1" ^| find "?"') do svn add "%%i"
REM for /f "tokens=2*" %%i in ('svn status "%1" ^| find "!"') do svn delete "%%i"

echo Commiting new build to SVN...
:: TODO auto generate a good changelist
"%SVN%" commit -m "Caveman2Cosmos %APPVEYOR_BUILD_VERSION%" --non-interactive --no-auth-cache --username %svn_user% --password %svn_pass%
POPD

REM 7z a -r -x!.svn "%release_prefix%-%APPVEYOR_BUILD_VERSION%.zip" "%build_dir%\*.*"
REM 7z a -x!.svn "%release_prefix%-CvGameCoreDLL-%APPVEYOR_BUILD_VERSION%.zip" "%build_dir%\Assets\CvGameCoreDLL.*"

echo Done!
exit /B 0

POPD