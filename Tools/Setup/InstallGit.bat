@echo off
PUSHD "%~dp0"
powershell -ExecutionPolicy Bypass -File "%~dp0.\DownloadAndInstallGit.ps1"
POPD
