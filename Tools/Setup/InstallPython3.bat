@echo off
PUSHD "%~dp0"

echo Installing Python 3 ...
powershell -ExecutionPolicy Bypass -File "%~dp0.\DownloadAndInstallPython3.ps1"
echo Updating pip ...
"%~dp0..\python\tools\python" -m pip install --upgrade pip --no-warn-script-location

POPD
