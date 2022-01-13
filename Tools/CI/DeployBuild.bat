@echo off

:: What is going on in this script?
:: 1. Immediately abort this script if we aren't in the designated release branch (it should only run there).
:: 2. Update the version number in the mod XML
:: 3. Compile FinalRelease DLL (and add source indexing)
:: 4. Fetch the latest SVN to a temp dir
:: 4. Build FPK patch against the last SVN FPKs
:: 5. Update the files in the SVN dir with our new ones
:: 6. Generate change logs
:: 7. Commit changed files to SVN
:: 8. Set tag on git containing version number and SVN commit

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
powershell -ExecutionPolicy Bypass -File "%root_dir%\Tools\CI\update-c2c-version.ps1"

:: INIT GIT WRITE ---------------------------------------------
powershell -ExecutionPolicy Bypass -File "%root_dir%\Tools\CI\InitGit.ps1"

:: COMPILE -----------------------------------------------------
echo Building FinalRelease DLL...
call "%root_dir%\Tools\_MakeDLL.bat" FinalRelease build deploy
if not errorlevel 0 (
    echo Building FinalRelease DLL failed, aborting deployment!
    exit /B 2
)
call "%root_dir%\Tools\_TrimFBuildCache.bat"

:: SOURCE INDEXING ---------------------------------------------
:source_indexing
call "%root_dir%\Tools\CI\DoSourceIndexing.bat"

:: CHECK OUT SVN -----------------------------------------------
echo Checking out SVN working copy for deployment...
call %SVN% --quiet checkout %svn_url% "%build_dir%"
if %ERRORLEVEL% neq 0 goto checkoutLoopSetup
goto OK

:checkoutLoopSetup
echo SVN checkout failed... Cleanup
call %SVN% --non-interactive cleanup "%build_dir%"
echo Make 25 more attempts...
set /a count = 0
set /a max = 25
:checkoutLoop
set /a count += 1
call %SVN% --quiet checkout %svn_url% "%build_dir%"
if %ERRORLEVEL% neq 0 (
	if %count% GTR %max% (
		echo SVN checkout failed, aborting...
		exit /B 3
	)
	echo Attempt %count% failed... cleanup
	call %SVN% --non-interactive cleanup "%build_dir%"
	goto checkoutLoop
)
:OK
echo Successfull checkout...

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
call xcopy "%build_dir%\Assets\*.FPK" "Assets" /Y
call xcopy "%build_dir%\Assets\fpklive_token.txt" "Assets" /Y

:fpk_live
echo Packing FPKs...
call "%root_dir%\Tools\FPKLive.exe"
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

:: SET TEMP GIT RELEASE TAG -----------------------------------
:: This is temporary so that the change log gets created
:: correctly (it uses origin tags I guess).
:: TODO: update chlog to not require this...
echo Setting release version build tag on git ...
call git tag -a %C2C_VERSION% %APPVEYOR_REPO_COMMIT% -m "%C2C_VERSION%" -f
call git push --tags

:: GENERATE NEW CHANGES LOG ------------------------------------
echo Generate SVN commit description...
call Tools\CI\git-chglog_windows_amd64.exe --output "%root_dir%\commit_desc.md" --config Tools\CI\.chglog\config.yml %C2C_VERSION%

echo Generate forum commit description...
call Tools\CI\git-chglog_windows_amd64.exe --output "%root_dir%\commit_desc.txt" --config Tools\CI\.chglog\config-bbcode.yml %C2C_VERSION%

:: GENERATE FULL CHANGELOG -------------------------------------
echo Update full SVN changelog ...
call Tools\CI\git-chglog_windows_amd64.exe --output "%build_dir%\CHANGELOG.md" --config Tools\CI\.chglog\config.yml
REM call github_changelog_generator --cache-file "github-changelog-http-cache" --cache-log "github-changelog-logger.log" -u caveman2cosmos --token %git_access_token% --future-release %C2C_VERSION% --release-branch %release_branch% --output "%build_dir%\CHANGELOG.md"

:: DELETE TEMP RELEASE TAG -------------------------------------
:: We delete it ASAP so it isn't left up if the build fails
:: below.
call git push origin --delete %C2C_VERSION%

:: DETECT SVN CHANGES ------------------------------------------
echo Detecting working copy changes...
PUSHD "%build_dir%"
call %SVN% status | findstr /R "^!" > ..\missing.list
for /F "tokens=* delims=! " %%A in (..\missing.list) do (svn delete "%%A")
del ..\missing.list 2>NUL
call %SVN% add * --force

:: COMMIT TO SVN -----------------------------------------------
echo Commiting new build to SVN...
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
call %SVN% --quiet update
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

:: Add the tag, this time annotated with our SVN ID
call git tag -a %C2C_VERSION% %APPVEYOR_REPO_COMMIT% -m "SVN-%svn_rev%" -f
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
