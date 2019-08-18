@echo off
PUSHD "%~dp0..\.."

echo C2C %APPVEYOR_BUILD_VERSION% DEPLOYMENT
echo.

echo Packing FPKs...
call Tools\PackFPKs.bat CI
if not errorlevel 0 (
    echo Packing FPKs failed, aborting deployment!
    exit /B 1
)

echo Building FinalRelease DLL...
call Tools\MakeDLLFinalRelease.bat
if not errorlevel 0 (
    echo Building FinalRelease DLL failed, aborting deployment!
    exit /B 2
)
call Tools\CI\DoSourceIndexing.bat

cd /d "%~dp0..\.."
if not exist "%build_dir%" goto :checkout
rmdir /Q /S "%build_dir%"

:checkout
echo Checking out SVN working copy for deployment...
svn checkout --username=%svn_user% %svn_url% "%build_dir%"

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
set "SVN=C:\Program Files\SlikSvn\bin\svn.exe"
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
"%SVN%" commit -m "C2C %APPVEYOR_BUILD_VERSION%" --username %svn_user% --password %svn_pass% --non-interactive
POPD

7z a -r -x!.svn "%release_prefix%-%APPVEYOR_BUILD_VERSION%.zip" "%build_dir%\*.*"
7z a -x!.svn "%release_prefix%-CvGameCoreDLL-%APPVEYOR_BUILD_VERSION%.zip" "%build_dir%\Assets\CvGameCoreDLL.*"

echo Done!
exit /B 0

POPD