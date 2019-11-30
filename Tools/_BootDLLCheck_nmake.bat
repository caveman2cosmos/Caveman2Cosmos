@echo off
PUSHD "%~dp0"

:: Call iterative build without staging the DLL (we couldn't anyway as Civ will have locked it)
call _MakeDLL.bat autobuild %1
if errorlevel 1 goto :build_failed
:: Compare the built DLL with the staged one
fc ..\Build\%1\CvGameCoreDLL.dll ..\Assets\CvGameCoreDLL.dll > nul
:: If there is no change then we can exit
if errorlevel 1 goto :changed
goto :no_change
:changed
:: If there is a change then kill Civ, stage the new DLL and restart it again
call :banner
echo [32m"******************************************************"[0m
echo [32m"*     _   _ ____  ____    _  _____ _____ ____  _     *"[0m
echo [32m"*    | | | |  _ \|  _ \  / \|_   _| ____|  _ \| |    *"[0m
echo [32m"*    | | | | |_) | | | |/ _ \ | | |  _| | | | | |    *"[0m
echo [32m"*    | |_| |  __/| |_| / ___ \| | | |___| |_| |_|    *"[0m
echo [32m"*     \___/|_|   |____/_/   \_\_| |_____|____/(_)    *"[0m
echo [32m"*                                                    *"[0m
echo [32m"******************************************************"[0m
echo   If you use Steam then you will need to run the game 
echo   again yourself!
echo.
timeout 5

taskkill /F /IM Civ4BeyondSword.exe
call _MakeDLL.bat build %1 stage

..\LaunchC2C.bat
goto :done

:no_change
echo No DLL changes ...
goto :done

:build_failed
call :banner
echo [31m"******************************************************"[0m
echo [31m"*         _   _ ____  ____    _  _____ _____         *"[0m
echo [31m"*        | | | |  _ \|  _ \  / \|_   _| ____|        *"[0m
echo [31m"*        | | | | |_) | | | |/ _ \ | | |  _|          *"[0m
echo [31m"*        | |_| |  __/| |_| / ___ \| | | |___         *"[0m
echo [31m"*         \___/|_|   |____/_/  _\_\_|_|_____|        *"[0m
echo [31m"*        |  ___/ \  |_ _| |   | ____|  _ \| |        *"[0m
echo [31m"*        | |_ / _ \  | || |   |  _| | | | | |        *"[0m
echo [31m"*        |  _/ ___ \ | || |___| |___| |_| |_|        *"[0m
echo [31m"*        |_|/_/   \_\___|_____|_____|____/(_)        *"[0m
echo [31m"*                                                    *"[0m
echo [31m"******************************************************"[0m
echo   You will need to fix the issues described in the 
echo   log ^^^^ up there, or find someone who can!
echo   Until the problem is fixed you will be running an
echo   out of date DLL, which will introduce instability,
echo   possibly crashing your game, or corrupting your 
echo   save at worst.
echo.
echo   Press any key to continue using the old DLL...
pause > nul
POPD
exit /B 1

:done
POPD
exit /B 0

:banner
echo [47m[30m"******************************************************"[0m
echo [47m[30m"* [0m   ___  [47m[30m                                           *"[0m
echo [47m[30m"* [0m  / __| [47m[30m __ _ __ __ ___  _ __   __ _  _ _          *"[0m
echo [47m[30m"* [0m | (__  [47m[30m/ _\`|\ V // -_)| '  \ / _\`|| ' \         *"[0m
echo [47m[30m"* [0m  \___| [47m[30m\__,_| \_/ \___||_|_|_|\__,_||_||_|        *"[0m
echo [47m[30m"* [0m        [47m[30m  ___   [0m   ___  [47m[30m                           *"[0m
echo [47m[30m"*          |_  )  [0m  / __| [47m[30m ___  ___ _ __   ___  ___  *"[0m
echo [47m[30m"*           / /   [0m | (__  [47m[30m/ _ \(_-<| '  \ / _ \(_-<  *"[0m
echo [47m[30m"*          /___|  [0m  \___| [47m[30m\___//__/|_|_|_|\___//__/  *"[0m
echo [47m[30m"*                 [0m        [47m[30m                           *"[0m
echo [47m[30m"******************************************************"[0m

exit /B 0