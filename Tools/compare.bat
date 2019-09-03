@echo off
PUSHD "%~dp0"

:: Call iterative build without staging the DLL (we couldn't anyway as Civ will have locked it)
:: Compare the built DLL with the staged one
fc ..\Build\Release\CvGameCoreDLL.dll ..\Assets\CvGameCoreDLL.dll > nul
:: If there is no change then we can exit
if errorlevel 1 goto :changed
goto :no_change
:changed
:: If there is a change then kill Civ, stage the new DLL and restart it again
echo DLL changes detected, restarting ...
taskkill /F /IM Civ4BeyondSword.exe
call _MakeDLL.bat build %1 stage
..\LaunchC2C.bat
goto :done

:no_change
echo No DLL changes ...
goto :done

:done
POPD
pause
