@echo off

echo Setting build tag on git ...

cd /d "%~dp0..\.."

git config --global credential.helper store
powershell -ExecutionPolicy Bypass -command "Add-Content '%USERPROFILE%\.git-credentials' 'https://%git_access_token%:x-oauth-basic@github.com`n'"
REM ps: Add-Content "$HOME\.git-credentials" "https://$($env:git_access_token):x-oauth-basic@github.com`n"
git config --global user.email "%git_email%"
git config --global user.name "%git_user%"

git checkout master
git tag -a %version% %APPVEYOR_REPO_COMMIT% -m "%version%"
git push --tags

exit /B 0
