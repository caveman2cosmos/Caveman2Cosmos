@echo off
PUSHD "%~dp0..\.."

SET version=v%APPVEYOR_BUILD_VERSION%-alpha


if "%APPVEYOR_REPO_BRANCH%" neq "%release_branch%" (
    echo Skipping deployment due to not being on release branch
    exit /b 0
)

if "%APPVEYOR_PULL_REQUEST_TITLE%" neq "" (
    echo Skipping deployment due to being a PR build
    exit /b 0
)

echo C2C %version% DEPLOYMENT
echo.

echo Packing FPKs...
call Tools\PackFPKs.bat CI
if %ERRORLEVEL% neq 0 (
    echo Packing FPKs failed, aborting deployment
    exit /B 1
)

if "%testing%"=="true" goto :source_indexing

echo Building FinalRelease DLL...
call Tools\MakeDLLFinalRelease.bat
if not errorlevel 0 (
    echo Building FinalRelease DLL failed, aborting deployment!
    exit /B 2
)

:source_indexing
call Tools\CI\DoSourceIndexing.bat

cd /d "%~dp0..\.."
set "root_dir=%cd%"
if not exist "%build_dir%" goto :checkout
rmdir /Q /S "%build_dir%"

:checkout
echo Checking out SVN working copy for deployment...
if "%testing%"=="true" goto :test_svn
svn checkout %svn_url% "%build_dir%"
goto :update_svn
:test_svn
svn checkout %svn_test_url% "%build_dir%"

:update_svn
:: HERE IS WHERE YOU ADJUST WHAT TO PUT IN THE BUILD
echo Updating SVN working copy from git...
set ROBOCOPY_FLAGS=/MIR /NFL /NDL /NJH /NJS /NS /NC
robocopy Assets "%build_dir%\Assets" %ROBOCOPY_FLAGS%
robocopy PrivateMaps "%build_dir%\PrivateMaps" %ROBOCOPY_FLAGS%
robocopy Resource "%build_dir%\Resource" %ROBOCOPY_FLAGS%
xcopy Caveman2Cosmos.ini "%build_dir%" /R /Y
xcopy "Caveman2Cosmos Config.ini" "%build_dir%" /R /Y

echo Update full SVN changelog ...
call github_changelog_generator --cache-file "github-changelog-http-cache" --cache-log "github-changelog-logger.log" -u caveman2cosmos --token %git_access_token% --future-release %version% --release-branch %release_branch% --output "%build_dir%\CHANGELOG.md"

echo Generate SVN commit description...
call github_changelog_generator --cache-file "github-changelog-http-cache" --cache-log "github-changelog-logger.log" -u caveman2cosmos --token %git_access_token% --future-release %version% --release-branch %release_branch% --unreleased-only --output "%root_dir%\commit_desc.md"

echo Detecting working copy changes...
PUSHD "%build_dir%"
set SVN=svn.exe
"%SVN%" status | findstr /R "^!" > ..\missing.list
for /F "tokens=* delims=! " %%A in (..\missing.list) do (svn delete "%%A")
del ..\missing.list 2>NUL
"%SVN%" add * --force

echo Commiting new build to SVN...
:: TODO auto generate a good changelist
"%SVN%" commit -F "%root_dir%\commit_desc.md" --non-interactive --no-auth-cache --username %svn_user% --password %svn_pass%
POPD

REM 7z a -r -x!.svn "%release_prefix%-%APPVEYOR_BUILD_VERSION%.zip" "%build_dir%\*.*"
REM 7z a -x!.svn "%release_prefix%-CvGameCoreDLL-%APPVEYOR_BUILD_VERSION%.zip" "%build_dir%\Assets\CvGameCoreDLL.*"

echo Setting build tag on git ...

git config --global credential.helper store
powershell -ExecutionPolicy Bypass -command "Add-Content '$HOME\.git-credentials' 'https://$($env:git_access_token):x-oauth-basic@github.com`n'"
REM ps: Add-Content "$HOME\.git-credentials" "https://$($env:git_access_token):x-oauth-basic@github.com`n"
git config --global user.email "%git_email%"
git config --global user.name "%git_user%"

git checkout master
git tag -a %version% %APPVEYOR_REPO_COMMIT% -m "%version%"
git push --tags

echo Done!
exit /B 0

POPD
