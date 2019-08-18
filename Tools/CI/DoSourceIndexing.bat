@echo off

PUSHD "%~dp0..\.."

if "%APPVEYOR_REPO_COMMIT%" == "" ( 
    :: Get current branch to variablef
    :: for /f %%i in ('git symbolic-ref --short HEAD') do set "GIT_BRANCH=%%i"
    call :get_git_sha
) else (
    set GIT_SHA=%APPVEYOR_REPO_COMMIT% 
)
rem for /f %%i in ('git config --global user.name') do set "GIT_USER=%%i"

:: Important: github expects correct casing for names, so verifyLocalRepo is imperitive.
powershell "& Tools\CI\github-sourceindexer.ps1 -dbgToolsPath '%cd%\Sources\deps\srcsrv' -symbolsFolder '%cd%\Assets' -userId 'caveman2cosmos' -repository 'Caveman2Cosmos' -commit %GIT_SHA% -sourcesRoot '%cd%' -verifyLocalRepo -verbose"
set errorlevel_saved=%errorlevel%

POPD

exit /B %errorlevel_saved%

:get_git_sha
for /f %%i in ('git rev-parse HEAD') do set "GIT_SHA=%%i"
exit /B 0
