@echo off

PUSHD "%~dp0..\.."

set GIT_SHA=%APPVEYOR_REPO_COMMIT% 

if "%GIT_SHA%" == "" ( 
    call :get_git_sha
)

rem for /f %%i in ('git config --global user.name') do set "GIT_USER=%%i"

:: Important: github expects correct casing for names, so verifyLocalRepo is imperitive. Add  -verbose for more detailed output
powershell -ExecutionPolicy Bypass -File ".\Tools\CI\github-sourceindexer.ps1" -dbgToolsPath "%cd%\Build\deps\srcsrv" -symbolsFolder "%cd%\Assets" -userId "caveman2cosmos" -repository "Caveman2Cosmos" -commit %GIT_SHA% -sourcesRoot "%cd%" -verifyLocalRepo
set errorlevel_saved=%errorlevel%

POPD

exit /B %errorlevel_saved%

:get_git_sha
:: Get current branch to variablef
:: for /f %%i in ('git symbolic-ref --short HEAD') do set "GIT_BRANCH=%%i"
for /f %%i in ('git rev-parse HEAD') do set "GIT_SHA=%%i"
exit /B 0
