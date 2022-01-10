@echo off

if "%1" neq "" (
    echo Usage error - this batch file doesn't take any arguments.
    echo To use autocorrect, run this batch file without arguments to start the server,
    echo and then drop text xml files onto DropXMLHere.bat
    pause
    exit /B 1
)

echo Checking for java install ...
where /Q java
if errorlevel 1 (
    echo Please install Java Runtime then try again!
    pause
    exit /B 1
)
PUSHD "%~dp0"

echo Downloading LanguageTool server ...
..\Utils\wget.exe -O LanguageTool.zip "https://languagetool.org/download/LanguageTool-4.7.zip" --quiet --show-progress --continue

echo Unzipping LanguageTool server ...
..\Utils\7z.exe x LanguageTool.zip -r -aos

echo Updating Python ...
call ..\Setup\InstallPython3.bat
set PYTHON="%~dp0..\python\tools\python"

echo Installing the required Python libraries ...
set PYTHONIOENCODING=UTF-8
%PYTHON% -m pip install pylanguagetool pyspellchecker lxml colorama --no-warn-script-location

echo Starting server ...
PUSHD "%~dp0\LanguageTool-4.7"
start cmd /c "java -cp languagetool-server.jar org.languagetool.server.HTTPServer --port 8081 --allow-origin '*' --config ..\language-tool.properties"
POPD

POPD
