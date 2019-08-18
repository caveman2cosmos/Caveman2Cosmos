@echo off
PUSHD "%~dp0..\.."

echo C2C %APPVEYOR_BUILD_VERSION% DEPLOYMENT
echo.

REM echo Packing FPKs...
REM call Tools\PackFPKs.bat CI
REM if not errorlevel 0 (
REM     echo Packing FPKs failed, aborting deployment!
REM     exit /B 1
REM )

REM echo Building FinalRelease DLL...
REM call Tools\MakeDLLFinalRelease.bat
REM if not errorlevel 0 (
REM     echo Building FinalRelease DLL failed, aborting deployment!
REM     exit /B 2
REM )
REM call Tools\CI\DoSourceIndexing.bat

cd /d "%~dp0..\.."
REM if not exist "%build_dir%" goto :checkout
REM rmdir /Q /S "%build_dir%"

REM :checkout
REM echo Checking out SVN working copy for deployment...
REM svn checkout --username=%svn_user% %svn_url% "%build_dir%"

REM :: HERE IS WHERE YOU ADJUST WHAT TO PUT IN THE BUILD
REM echo Updating SVN working copy from git...
REM set ROBOCOPY_FLAGS=/MIR /NFL /NDL /NJH /NJS /NS /NC
REM robocopy Assets "%build_dir%\Assets" %ROBOCOPY_FLAGS%
REM robocopy PrivateMaps "%build_dir%\PrivateMaps" %ROBOCOPY_FLAGS%
REM robocopy Resource "%build_dir%\Resource" %ROBOCOPY_FLAGS%
REM xcopy Caveman2Cosmos.ini "%build_dir%" /R /Y
REM xcopy "Caveman2Cosmos Config.ini" "%build_dir%" /R /Y

REM echo Detecting working copy changes...
REM PUSHD "%build_dir%"
REM set "SVN=C:\Program Files\SlikSvn\bin\svn.exe"
REM "%SVN%" status | findstr /R "^!" > ..\missing.list
REM for /F "tokens=* delims=! " %%A in (..\missing.list) do (svn delete "%%A")
REM del ..\missing.list 2>NUL
REM "%SVN%" add * --force
REM REM svn status | findstr /R "^?" > ..\added.list
REM REM for /F "tokens=* delims=? " %%A in (..\added.list) do (svn add "%%A")
REM REM del ..\added.list 2>NUL

REM REM for /f "tokens=2*" %%i in ('svn status "%1" ^| find "?"') do svn add "%%i"
REM REM for /f "tokens=2*" %%i in ('svn status "%1" ^| find "!"') do svn delete "%%i"


REM echo Commiting new build to SVN...
REM "%SVN%" commit -m "C2C %APPVEYOR_BUILD_VERSION%" --username %svn_user% --password %svn_pass% --non-interactive
REM POPD

7z a -r -x!.svn "%release_prefix%-%APPVEYOR_BUILD_VERSION%.zip" "%build_dir%\*.*"
7z a -x!.svn "%release_prefix%-CvGameCoreDLL-%APPVEYOR_BUILD_VERSION%.zip" "%build_dir%\Assets\CvGameCoreDLL.*"

echo Done!
exit /B 0

POPD