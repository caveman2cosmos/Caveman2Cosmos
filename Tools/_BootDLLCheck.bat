@echo off
PUSHD "%~dp0"

:: Call iterative build without staging the DLL (we couldn't anyway as Civ will have locked it)
call _MakeDLL.bat build %1
:: Compare the built DLL with the staged one
fc ..\Build\%1\CvGameCoreDLL.dll ..\Assets\CvGameCoreDLL.dll > nul
:: If there is no change then we can exit
if errorlevel 1 goto :changed
goto :no_change
:changed
:: If there is a change then kill Civ, stage the new DLL and restart it again
echo.
echo.
echo ********************************************************************************
echo "   ___                                     ___  ___                           "
echo "  / __| __ _ __ __ ___  _ __   __ _  _ _  |_  )/ __| ___  ___ _ __   ___  ___ "
echo " | (__ / _\`|\ V // -_)| '  \ / _\`|| ' \  / /| (__ / _ \(_-<| '  \ / _ \(_-< "
echo "  \___|\__,_| \_/ \___||_|_|_|\__,_||_||_|/___|\___|\___//__/|_|_|_|\___//__/ "
echo "                                                                              "
echo ********************************************************************************
echo.
echo   %1 DLL UPDATED!
echo.
echo   Press any key to restart the game and apply the changes ...
echo.
echo   (If you use Steam then you will need to run the game again yourself)
echo.

pause > nul
taskkill /F /IM Civ4BeyondSword.exe
call _MakeDLL.bat build %1 stage

..\LaunchC2C.bat
goto :done

:no_change
echo No DLL changes ...
goto :done

:done
POPD
