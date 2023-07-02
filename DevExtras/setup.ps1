$curdir = (resolve-path .).Path
$sh = new-object -com 'Shell.Application'
$sh.ShellExecute('powershell', '-NoExit -Command [System.Environment]::SetEnvironmentVariable(\"PATH\", $Env:Path + \";' + $curdir + '\bin\", \"Machine\")', '', 'runas')