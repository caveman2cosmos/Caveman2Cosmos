if([Environment]::Is64BitOperatingSystem) {
    & "..\Utils\nuget.exe" install python -ExcludeVersion -OutputDirectory ..
}
else {
    & "..\Utils\nuget.exe" install pythonx86 -ExcludeVersion -OutputDirectory ..
}
