[string[]]$changes = &git whatchanged --pretty=format:%at ..\..\Sources
$mtime = [DateTime]::Now;
[string]$change = $null;
foreach($change in $changes)
{
    if($change.Length -eq 0) { continue; }
    if($change[0] -eq ":")
    {
        $parts = $change.Split("`t");
        $file = $parts[$parts.Length - 1];
        if([System.IO.File]::Exists($file))
        {
            [System.IO.File]::SetLastWriteTimeUtc($file, $mtime);
        }
    }
    else
    {
        #get timestamp
        $mtime = [DateTimeOffset]::FromUnixTimeSeconds([Int64]::Parse($change)).DateTime;
    }
}
