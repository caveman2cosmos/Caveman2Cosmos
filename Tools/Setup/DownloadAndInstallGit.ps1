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

function downloadFile($url, $targetFile)
{ 
    "Downloading $url" 
    [Net.ServicePointManager]::SecurityProtocol = "tls12, tls11, tls"
    $uri = New-Object "System.Uri" "$url" 
    $request = [System.Net.HttpWebRequest]::Create($uri) 
    $request.set_Timeout(15000) #15 second timeout 
    $response = $request.GetResponse() 
    $totalLength = [System.Math]::Floor($response.get_ContentLength()/1024) 
    $responseStream = $response.GetResponseStream() 
    $targetStream = New-Object -TypeName System.IO.FileStream -ArgumentList $targetFile, Create 
    $buffer = new-object byte[] 10KB 
    $count = $responseStream.Read($buffer,0,$buffer.length) 
    $downloadedBytes = $count 
    while ($count -gt 0) 
    { 
        [System.Console]::CursorLeft = 0 
        [System.Console]::Write("Downloaded {0}K of {1}K", [System.Math]::Floor($downloadedBytes/1024), $totalLength) 
        $targetStream.Write($buffer, 0, $count) 
        $count = $responseStream.Read($buffer,0,$buffer.length) 
        $downloadedBytes = $downloadedBytes + $count 
    } 
    "`nFinished Download" 
    $targetStream.Flush()
    $targetStream.Close() 
    $targetStream.Dispose() 
    $responseStream.Dispose() 
}

$gitexe = FindGitExe
if (!$gitexe) {
    "Git command line not found! It will now be downloaded and installed for you, if you wish to cancel then close the window."
    "Otherwise press any key to continue with download and install..."
    [void][System.Console]::ReadKey($true)

    if([Environment]::Is64BitOperatingSystem) {
        downloadFile "https://github.com/git-for-windows/git/releases/download/v2.23.0.windows.1/Git-2.23.0-64-bit.exe" "git_installer.exe"
    }
    else {
        downloadFile "https://github.com/git-for-windows/git/releases/download/v2.23.0.windows.1/Git-2.23.0-32-bit.exe" "git_installer.exe"
    }
    
    "Waiting for installer to finish..."
    Start-Process -FilePath "git_installer.exe" -ArgumentList "/SILENT /NORESTART /NOCANCEL /SP- /CLOSEAPPLICATIONS /RESTARTAPPLICATIONS /LOADINF=git_install.inf" -Wait
}
