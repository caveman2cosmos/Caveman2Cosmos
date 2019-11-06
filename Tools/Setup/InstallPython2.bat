@echo off
PUSHD "%~dp0"

echo Installing Python 2 ...
powershell -ExecutionPolicy Bypass -File "%~dp0.\DownloadAndInstallPython2.ps1"
echo Updating pip ...
"%~dp0..\python2\tools\python" -m pip install --upgrade pip --no-warn-script-location
"%~dp0..\python2\tools\python" -m pip install lxml

POPD
