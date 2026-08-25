param(
    [string]$DistDir = "dist",
    [string]$OutMsi = "OpenWordPad-Windows-x64-Setup.msi",
    [string]$Version = "1.3.3.0"
)

$ErrorActionPreference = "Stop"

Write-Host "=== Building MSI Installer ==="
Write-Host "DistDir: $DistDir"
Write-Host "OutMsi: $OutMsi"
Write-Host "Version: $Version"

# Find WiX Toolset v3 or v4
$wix3Bin = ""
if ($env:WIX -and (Test-Path "$env:WIX\bin\heat.exe")) {
    $wix3Bin = "$env:WIX\bin"
} elseif (Test-Path "C:\Program Files (x86)\WiX Toolset v3.11\bin\heat.exe") {
    $wix3Bin = "C:\Program Files (x86)\WiX Toolset v3.11\bin"
}

if ($wix3Bin -ne "") {
    Write-Host "Found WiX Toolset v3 at: $wix3Bin"
    $env:PATH = "$wix3Bin;$env:PATH"
    
    $wixUI = "$wix3Bin\WixUIExtension.dll"

    Write-Host "1. Harvesting files with heat..."
    & "$wix3Bin\heat.exe" dir $DistDir -cg AppFiles -dr INSTALLFOLDER -scom -sreg -srd -var var.DistDir -gg -out BundleFiles.wxs

    Write-Host "2. Compiling with candle..."
    & "$wix3Bin\candle.exe" -arch x64 "-dDistDir=$DistDir" -ext "$wixUI" installer\windows\OpenWordPad.wxs BundleFiles.wxs

    Write-Host "3. Linking with light..."
    & "$wix3Bin\light.exe" -ext "$wixUI" -sval -out $OutMsi OpenWordPad.wixobj BundleFiles.wixobj

    if (Test-Path $OutMsi) {
        Write-Host "Successfully generated MSI: $OutMsi (Size: $((Get-Item $OutMsi).Length) bytes)"
        exit 0
    } else {
        Write-Error "MSI file was not produced."
    }
}

Write-Host "WiX v3 not found. Trying WiX v4 CLI..."
dotnet tool install --global wix --version 4.0.1 2>$null
wix build installer\windows\OpenWordPad.wxs -d DistDir=$DistDir -o $OutMsi
