@echo off

echo Checking for java install ...
where /Q java
if errorlevel 1 (
    echo Please install Java Runtime then try again!
    pause
    exit /B 1
)
PUSHD "%~dp0"

echo Downloading LanguageTool server ...
..\Utils\wget.exe -O LanguageTool.zip "https://languagetool.org/download/LanguageTool-stable.zip" --quiet --show-progress --continue

echo Unzipping LanguageTool server ...
..\Utils\7z.exe x LanguageTool.zip -r -aos

echo Updating Python ...
call ..\Setup\InstallPython3.bat
set PYTHON="%~dp0..\python\tools\python"

echo Installing the required Python libraries ...
set PYTHONIOENCODING=UTF-8
%PYTHON% -m pip install pylanguagetool pyspellchecker lxml colorama --no-warn-script-location
REM %PYTHON% -m pip install pyspellchecker
REM %PYTHON% -m pip install lxml
REM %PYTHON% -m pip install colorama

REM We keep the dict uncompressed as it isn't that big and git will handle updates to it better
REM echo Unzipping the spell-check dictionary...
REM ..\Utils\7z.exe x dicts\en_full.7z -r -aos

echo Starting server ...
PUSHD "%~dp0\LanguageTool-4.6"
start cmd /c "java -cp languagetool-server.jar org.languagetool.server.HTTPServer --port 8081 --allow-origin '*'"
POPD

POPD
