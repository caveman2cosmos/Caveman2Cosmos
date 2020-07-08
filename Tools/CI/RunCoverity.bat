@echo off

call "%~dp0..\InstallDeps.bat"
SET "PATH=%PATH%;C:\Programs\cov-analysis-win64-2019.03\bin"

PUSHD "%~pd0..\..\Sources"
call "..\Build\deps\Microsoft Visual C++ Toolkit 2003\vcvarsall.bat"

cov-configure --compiler "..\Build\deps\Microsoft Visual C++ Toolkit 2003\bin\cl.exe" --comptype msvc
cov-build --dir cov-int "%~pd0..\MakeDLLDebug.bat"


POPD
