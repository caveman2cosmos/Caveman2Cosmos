@echo off

PUSHD "%~dp0"

echo Downloading ...
..\Utils\wget.exe -O LanguageTool.zip "https://languagetool.org/download/LanguageTool-stable.zip" --quiet --show-progress --continue
echo Unzipping ...
..\Utils\7z.exe x LanguageTool.zip -r -aos
echo Installing the Python library ...
set PYTHONIOENCODING=UTF-8
pip install pylanguagetool
pip install lxml
pip install colorama

PUSHD "%~dp0\LanguageTool-4.6"
start cmd /c "java -cp languagetool-server.jar org.languagetool.server.HTTPServer --port 8081 --allow-origin '*'"
POPD

POPD
