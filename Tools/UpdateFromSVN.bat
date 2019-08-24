@echo off

PUSHD "%~dp0.."

REM set "svn_dir=%~1"
set svn_dir=C:\Users\billw\Documents\SVN\Caveman2Cosmos_trunk2
set "git_dir=%cd%"
set "svn_assets_dir=%svn_dir%\Assets"
set "git_unpacked_dir=%git_dir%\UnpackedArt"
set "git_assets_dir=%git_dir%\Assets"

call Tools\PakBuild /I="%svn_assets_dir%" /O="%git_unpacked_dir%" /U
xcopy "%svn_assets_dir%\art" "%git_unpacked_dir%\art" /S /Y /R /EXCLUDE:svn_copy_exclude_list.txt
xcopy "%svn_dir%\Sources" "%git_dir%\Sources" /S /Y /R
xcopy "%svn_dir%\Resource" "%git_dir%\Resource" /S /Y /R
xcopy "%svn_assets_dir%\Config" "%git_assets_dir%\Config" /S /Y /R
xcopy "%svn_assets_dir%\Modules" "%git_assets_dir%\Modules" /S /Y /R
xcopy "%svn_assets_dir%\Python" "%git_assets_dir%\Python" /S /Y /R
xcopy "%svn_assets_dir%\res" "%git_assets_dir%\res" /S /Y /R
xcopy "%svn_assets_dir%\Sounds" "%git_assets_dir%\Sounds" /S /Y /R
xcopy "%svn_assets_dir%\XML" "%git_assets_dir%\XML" /S /Y /R

POPD

pause
