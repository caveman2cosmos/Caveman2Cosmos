@echo off

if "%APPVEYOR_REPO_BRANCH%" neq "%release_branch%" (
    echo Skipping deployment due to not being on release branch
    exit /b 0
)

if "%APPVEYOR_PULL_REQUEST_TITLE%" neq "" (
    echo Skipping deployment due to being a PR build
    exit /b 0
)

PUSHD "%~dp0..\.."
SET version=v%APPVEYOR_BUILD_VERSION%-alpha
SET "root_dir=%cd%"
if not exist "%build_dir%" goto :skip_delete
rmdir /Q /S "%build_dir%"
:skip_delete

echo C2C %version% DEPLOYMENT
echo.

:: INIT GIT WRITE ---------------------------------------------
powershell -ExecutionPolicy Bypass -File "%~dp0\InitGit.ps1"

:: PACK FPKS ---------------------------------------------------
echo Packing FPKs...
call Tools\PackFPKs.bat CI
if %ERRORLEVEL% neq 0 (
    echo Packing FPKs failed, aborting deployment
    exit /B 1
)

:: SET GIT RELEASE TAG -----------------------------------------
echo Setting release version build tag on git ...
git tag -a %version% %APPVEYOR_REPO_COMMIT% -m "%version%"
git push --tags

:: COMPILE -----------------------------------------------------
echo Building FinalRelease DLL...
call Tools\MakeDLLFinalRelease.bat
if not errorlevel 0 (
    echo Building FinalRelease DLL failed, aborting deployment!
    exit /B 2
)

:: SOURCE INDEXING ---------------------------------------------
:source_indexing
call Tools\CI\DoSourceIndexing.bat

:: CHECK OUT SVN -----------------------------------------------
echo Checking out SVN working copy for deployment...
svn checkout %svn_url% "%build_dir%"

:: STAGE TO SVN ------------------------------------------------
:: HERE IS WHERE YOU ADJUST WHAT TO PUT IN THE BUILD
echo Updating SVN working copy from git...
set ROBOCOPY_FLAGS=/MIR /NFL /NDL /NJH /NJS /NS /NC
robocopy Assets "%build_dir%\Assets" %ROBOCOPY_FLAGS%
robocopy PrivateMaps "%build_dir%\PrivateMaps" %ROBOCOPY_FLAGS%
robocopy Resource "%build_dir%\Resource" %ROBOCOPY_FLAGS%
xcopy Caveman2Cosmos.ini "%build_dir%" /R /Y
xcopy "Caveman2Cosmos Config.ini" "%build_dir%" /R /Y
xcopy "C2C.ico" "%build_dir%" /R /Y
xcopy "CIV_C2C.ico" "%build_dir%" /R /Y

:: GENERATE NEW CHANGES LOG ------------------------------------
echo Generate SVN commit description...
call Tools\CI\git-chglog_windows_amd64.exe --output "%root_dir%\commit_desc.md" --config Tools\CI\.chglog\config.yml %version%

:: GENERATE FULL CHANGELOG -------------------------------------
echo Update full SVN changelog ...
call Tools\CI\git-chglog_windows_amd64.exe --output "%build_dir%\CHANGELOG.md" --config Tools\CI\.chglog\config.yml
REM call github_changelog_generator --cache-file "github-changelog-http-cache" --cache-log "github-changelog-logger.log" -u caveman2cosmos --token %git_access_token% --future-release %version% --release-branch %release_branch% --output "%build_dir%\CHANGELOG.md"

:: DETECT SVN CHANGES ------------------------------------------
echo Detecting working copy changes...
PUSHD "%build_dir%"
set SVN=svn.exe
"%SVN%" status | findstr /R "^!" > ..\missing.list
for /F "tokens=* delims=! " %%A in (..\missing.list) do (svn delete "%%A")
del ..\missing.list 2>NUL
"%SVN%" add * --force

:: COMMIT TO SVN -----------------------------------------------
echo Commiting new build to SVN...
:: TODO auto generate a good changelist
"%SVN%" commit -F "%root_dir%\commit_desc.md" --non-interactive --no-auth-cache --username %svn_user% --password %svn_pass%

:: SET SVN RELEASE TAG -----------------------------------------
echo Setting SVN commit tag on git ...
svnversion > svn_rev.txt
set /p %svn_rev%=<svn_rev.txt

POPD

git tag -a SVN-%svn_rev% %APPVEYOR_REPO_COMMIT% -m "SVN-%svn_rev%"
git push --tags

REM 7z a -r -x!.svn "%release_prefix%-%APPVEYOR_BUILD_VERSION%.zip" "%build_dir%\*.*"
REM 7z a -x!.svn "%release_prefix%-CvGameCoreDLL-%APPVEYOR_BUILD_VERSION%.zip" "%build_dir%\Assets\CvGameCoreDLL.*"

echo Done!
exit /B 0

POPD
