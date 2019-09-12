Add-Type -AssemblyName System.Web
$escaped_path = [System.Web.HttpUtility]::HtmlEncode($env:BTS_DIR)
(Get-Content '..\Sources\C2C (VS2019).vcxproj.user.template') -replace '{BTSDIR}', $escaped_path | Out-File -encoding ASCII '..\Sources\C2C (VS2019).vcxproj.user'