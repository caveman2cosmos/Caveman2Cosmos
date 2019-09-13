@echo off

PUSHD "%~dp0"

echo Downloading ...
..\Utils\wget.exe -O LanguageTool.zip "https://languagetool.org/download/LanguageTool-stable.zip" --quiet --show-progress --continue
echo Unzipping ...
..\Utils\7z.exe x LanguageTool.zip -r -aos
echo Installing the Python libraries ...
set PYTHONIOENCODING=UTF-8
pip install pylanguagetool
pip install pyspellchecker
pip install lxml
pip install colorama

REM We keep the dict uncompressed as it isn't that big and git will handle updates to it better
REM echo Unzipping the spell-check dictionary...
REM ..\Utils\7z.exe x dicts\en_full.7z -r -aos


PUSHD "%~dp0\LanguageTool-4.6"
start cmd /c "java -cp languagetool-server.jar org.languagetool.server.HTTPServer --port 8081 --allow-origin '*'"
POPD

POPD
