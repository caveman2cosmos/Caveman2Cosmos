@echo off

PUSHD "%~dp0.."

:: Get current branch to variablef
for /f %%i in ('git symbolic-ref --short HEAD') do set "GIT_BRANCH=%%i"
rem for /f %%i in ('git config --global user.name') do set "GIT_USER=%%i"

powershell "& Tools\github-sourceindexer.ps1 -dbgToolsPath '%cd%\Sources\deps\srcsrv' -symbolsFolder '%cd%\Assets' -userId 'caveman2cosmos' -repository 'Caveman2Cosmos' -branch '%GIT_BRANCH%' -sourcesRoot '%cd%\Sources' -ignoreUnknown -verbose"

POPD