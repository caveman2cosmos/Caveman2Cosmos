
$xml_path = Resolve-Path "..\..\Assets\XML\A_New_Dawn_GlobalDefines.xml"
[xml]$xml = Get-Content $xml_path

foreach ($item in $xml.Civ4Defines.ChildNodes) {
    if ($item.DefineName -eq "C2C_VERSION") {
        $item.DefineTextVal = "$($env:C2C_VERSION)"
    }
}

$xml.Save($xml_path)
