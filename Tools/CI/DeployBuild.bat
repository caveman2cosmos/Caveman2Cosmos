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
SET C2C_VERSION=v%APPVEYOR_BUILD_VERSION%
SET "root_dir=%cd%"
set SVN=svn.exe
if not exist "%build_dir%" goto :skip_delete
rmdir /Q /S "%build_dir%"

:skip_delete

echo C2C %C2C_VERSION% DEPLOYMENT
echo.

:: WRITE VERSION TO XML ---------------------------------------
powershell -ExecutionPolicy Bypass -File "%~dp0\update-c2c-version.ps1"

:: INIT GIT WRITE ---------------------------------------------
powershell -ExecutionPolicy Bypass -File "%~dp0\InitGit.ps1"

REM :: SET GIT RELEASE TAG -----------------------------------------
REM echo Setting release version build tag on git ...
REM git tag -a %C2C_VERSION% %APPVEYOR_REPO_COMMIT% -m "%C2C_VERSION%"
REM git push --tags

:: COMPILE -----------------------------------------------------
echo Building FinalRelease DLL...
call "Tools\_MakeDLL.bat" autobuild FinalRelease
if not errorlevel 0 (
    echo Building FinalRelease DLL failed, aborting deployment!
    exit /B 2
)

:: SOURCE INDEXING ---------------------------------------------
:source_indexing
call Tools\CI\DoSourceIndexing.bat

:: CHECK OUT SVN -----------------------------------------------
echo Checking out SVN working copy for deployment...
call %SVN% checkout %svn_url% "%build_dir%"
if %ERRORLEVEL% neq 0 (
    call %SVN% cleanup --non-interactive
    call %SVN% checkout %svn_url% "%build_dir%"
    if %ERRORLEVEL% neq 0 (
        echo SVN checkout failed, aborting...
        exit /B 3
    )
)

:: PACK FPKS ---------------------------------------------------
:: We copy built FPKs and the fpklive token back from SVN 
:: so we can build a patch FPK against them. This reduces how
:: much we need to push back to SVN, and how much players
:: need to sync
echo Copying FPKs from SVN...

:: Only copy existing FPKs if we didn't request clean FPK build
if not "%APPVEYOR_REPO_COMMIT_MESSAGE:FPKCLEAN=%"=="%APPVEYOR_REPO_COMMIT_MESSAGE%" (
    goto :fpk_live
)
xcopy "%build_dir%\Assets\*.FPK" "Assets" /Y
xcopy "%build_dir%\Assets\fpklive_token.txt" "Assets" /Y

:fpk_live
echo Packing FPKs...
call Tools\FPKLive.exe
if %ERRORLEVEL% neq 0 (
    echo Packing FPKs failed, aborting deployment
    exit /B 1
)

:: STAGE TO SVN ------------------------------------------------
:: HERE IS WHERE YOU ADJUST WHAT TO PUT IN THE BUILD
echo Updating SVN working copy from git...
set ROBOCOPY_FLAGS=/MIR /NFL /NDL /NJH /NJS /NS /NC
robocopy Assets "%build_dir%\Assets" %ROBOCOPY_FLAGS%
robocopy PrivateMaps "%build_dir%\PrivateMaps" %ROBOCOPY_FLAGS%
robocopy Resource "%build_dir%\Resource" %ROBOCOPY_FLAGS%
robocopy Docs "%build_dir%\Docs" %ROBOCOPY_FLAGS%
xcopy "Caveman2Cosmos.ini" "%build_dir%" /R /Y
xcopy "Caveman2Cosmos Config.ini" "%build_dir%" /R /Y
xcopy "C2C1.ico" "%build_dir%" /R /Y
xcopy "C2C2.ico" "%build_dir%" /R /Y
xcopy "C2C3.ico" "%build_dir%" /R /Y
xcopy "C2C4.ico" "%build_dir%" /R /Y
xcopy "Tools\CI\C2C.bat" "%build_dir%" /R /Y

:: GENERATE NEW CHANGES LOG ------------------------------------
echo Generate SVN commit description...
call Tools\CI\git-chglog_windows_amd64.exe --output "%root_dir%\commit_desc.md" --config Tools\CI\.chglog\config.yml %C2C_VERSION%

echo Generate forum commit description...
call Tools\CI\git-chglog_windows_amd64.exe --output "%root_dir%\commit_desc.txt" --config Tools\CI\.chglog\config-bbcode.yml %C2C_VERSION%

:: GENERATE FULL CHANGELOG -------------------------------------
echo Update full SVN changelog ...
call Tools\CI\git-chglog_windows_amd64.exe --output "%build_dir%\CHANGELOG.md" --config Tools\CI\.chglog\config.yml
REM call github_changelog_generator --cache-file "github-changelog-http-cache" --cache-log "github-changelog-logger.log" -u caveman2cosmos --token %git_access_token% --future-release %C2C_VERSION% --release-branch %release_branch% --output "%build_dir%\CHANGELOG.md"

:: DETECT SVN CHANGES ------------------------------------------
echo Detecting working copy changes...
PUSHD "%build_dir%"
call %SVN% status | findstr /R "^!" > ..\missing.list
for /F "tokens=* delims=! " %%A in (..\missing.list) do (svn delete "%%A")
del ..\missing.list 2>NUL
call %SVN% add * --force

:: COMMIT TO SVN -----------------------------------------------
echo Commiting new build to SVN...
REM %SVN% commit -F "%root_dir%\commit_desc.md" --non-interactive --no-auth-cache --username %svn_user% --password %svn_pass%
call %SVN% commit -F "%root_dir%\commit_desc.md" --non-interactive --no-auth-cache --username %svn_user% --password %svn_pass%
if %ERRORLEVEL% neq 0 (
    call %SVN% cleanup --non-interactive
    call %SVN% commit -F "%root_dir%\commit_desc.md" --non-interactive --no-auth-cache --username %svn_user% --password %svn_pass%
    if %ERRORLEVEL% neq 0 (
        echo SVN commit failed, aborting...
        exit /B 3
    )
)

:: REFRESH SVN -------------------------------------------------
:: Ensuring that the svnversion call below will give a clean 
:: revision number
echo Refreshing SVN working copy...
REM %SVN% update
call %SVN% update
if %ERRORLEVEL% neq 0 (
    call %SVN% cleanup --non-interactive
    call %SVN% update
    if %ERRORLEVEL% neq 0 (
        echo SVN update failed, aborting...
        exit /B 3
    )
)

:: SET RELEASE TAG -----------------------------------------
echo Setting SVN commit tag on git ...
for /f "delims=" %%a in ('svnversion') do @set svn_rev=%%a

POPD

call git tag -a %C2C_VERSION% %APPVEYOR_REPO_COMMIT% -m "SVN-%svn_rev%"
call git push --tags

REM 7z a -r -x!.svn "%release_prefix%-%APPVEYOR_BUILD_VERSION%.zip" "%build_dir%\*.*"
REM 7z a -x!.svn "%release_prefix%-CvGameCoreDLL-%APPVEYOR_BUILD_VERSION%.zip" "%build_dir%\Assets\CvGameCoreDLL.*"

POPD

echo FORUM COMMIT MESSAGE ----------------------------------------------------------
echo -------------------------------------------------------------------------------
echo.
echo [size=6][b]SVN-%svn_rev%[/b][/size]
type "%root_dir%\commit_desc.txt"
echo.
echo -------------------------------------------------------------------------------
echo -------------------------------------------------------------------------------

echo Done!

exit /B 0
