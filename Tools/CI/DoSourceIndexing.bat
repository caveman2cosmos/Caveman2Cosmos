@echo off

PUSHD "%~dp0..\.."

:: Get current branch to variablef
for /f %%i in ('git symbolic-ref --short HEAD') do set "GIT_BRANCH=%%i"
for /f %%i in ('git rev-parse HEAD') do set "GIT_SHA=%%i"
rem for /f %%i in ('git config --global user.name') do set "GIT_USER=%%i"

:: Important: github expects correct casing for names, so verifyLocalRepo is imperitive.
powershell "& Tools\CI\github-sourceindexer.ps1 -dbgToolsPath '%cd%\Sources\deps\srcsrv' -symbolsFolder '%cd%\Assets' -userId 'caveman2cosmos' -repository 'Caveman2Cosmos' -commit %GIT_SHA% -sourcesRoot '%cd%' -verifyLocalRepo -verbose"

POPD
