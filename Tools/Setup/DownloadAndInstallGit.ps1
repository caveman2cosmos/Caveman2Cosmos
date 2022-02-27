function FindGitExe {
    if (![String]::IsNullOrEmpty($gitPath)) {
        return $gitPath
    }
  
    $gitPath = Get-Command git.exe -ErrorAction SilentlyContinue| Select-Object -ExpandProperty Definition
    if (![String]::IsNullOrEmpty($gitPath)) {
        return $gitPath
    }

    $suffix = "\git\bin\git.exe"
    
    $gitexe = ${env:ProgramFiles} + $suffix
    if (Test-Path $gitexe) {
        return $gitexe
    }
    
    if( [IntPtr]::size -eq 4 ) {
        return $null
    }
    
    $gitexe = ${env:ProgramFiles(x86)} + $suffix
    if (Test-Path $gitexe) {
        return $gitexe
    }
    
    return $null
}

$gitexe = FindGitExe
if (!$gitexe) {
    "Git command line not found! It will now be downloaded and installed for you, if you wish to cancel then close the window."
    "(NOTE: You may need to restart Steam for changes to take effect)"
    "Press any key to continue with installation of git..."
    [void][System.Console]::ReadKey($true)

    if([Environment]::Is64BitOperatingSystem) {
        & "..\Utils\wget.exe" -O "git_installer.exe" "https://github.com/git-for-windows/git/releases/download/v2.34.1.windows.1/Git-2.34.1-64-bit.exe" --quiet --show-progress
    }
    else {
        & ".\Utils\wget.exe" -O "git_installer.exe" "https://github.com/git-for-windows/git/releases/download/v2.34.1.windows.1/Git-2.34.1-32-bit.exe" --quiet --show-progress
    }
    
    "Waiting for installer to finish..."
    Start-Process -FilePath "git_installer.exe" -ArgumentList "/SILENT /NORESTART /NOCANCEL /SP- /CLOSEAPPLICATIONS /RESTARTAPPLICATIONS /LOADINF=git_install.inf" -Wait
}
