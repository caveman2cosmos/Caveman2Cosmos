@echo off

PUSHD "%~dp0.."

REM set "svn_dir=%~1"
set svn_dir=C:\Users\billw\Documents\SVN\Caveman2Cosmos_trunk2
set "git_dir=%cd%"
set "svn_assets_dir=%svn_dir%\Assets"
set "git_assets_dir=%git_dir%\UnpackedArt"

call Tools\PakBuild /I="%svn_assets_dir%" /O="%git_assets_dir%" /U
xcopy "%svn_assets_dir%\art" "%git_assets_dir%" /S /Y /R /EXCLUDE:svn_copy_exclude_list.txt
xcopy "%svn_dir%\Sources" "%git_dir%\Sources" /S /Y /R
xcopy "%svn_dir%\Resource" "%git_dir%\Resource" /S /Y /R
xcopy "%svn_assets_dir%" "%git_assets_dir%" /S /Y /R /EXCLUDE:svn_copy_exclude_list2.txt

POPD

pause
