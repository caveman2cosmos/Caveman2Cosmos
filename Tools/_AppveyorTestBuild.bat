@echo off

if "%APPVEYOR_REPO_BRANCH%" equ "%release_branch%" (
	echo Skipping test build due to being on release branch
	exit /b 0
)
call "%APPVEYOR_BUILD_FOLDER%\Tools\_MakeDLL.bat" Debug build
call "%APPVEYOR_BUILD_FOLDER%\Tools\_MakeDLL.bat" Release build
call "%APPVEYOR_BUILD_FOLDER%\Tools\_TrimFBuildCache.bat"
