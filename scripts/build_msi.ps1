param(
    [string]$DistDir = "dist",
    [string]$OutMsi = "OpenWordPad-Windows-x64-Setup.msi",
    [string]$Version = "1.3.3.0"
)

Write-Host "=== Starting Windows Installer Build ==="
Write-Host "Current Directory: $(Get-Location)"
Write-Host "DistDir: $DistDir"

# 1. Build MSI with WiX if available
$wixBin = ""
if ($env:WIX -and (Test-Path "$env:WIX\bin\candle.exe")) {
    $wixBin = "$env:WIX\bin"
} elseif (Test-Path "C:\Program Files (x86)\WiX Toolset v3.11\bin\candle.exe") {
    $wixBin = "C:\Program Files (x86)\WiX Toolset v3.11\bin"
}

if ($wixBin -ne "") {
    Write-Host "WiX Toolset v3 found at: $wixBin"
    $env:PATH = "$wixBin;$env:PATH"
    $wixUI = "$wixBin\WixUIExtension.dll"

    try {
        Write-Host "[1/3] Running heat.exe..."
        & "$wixBin\heat.exe" dir $DistDir -cg AppFiles -dr INSTALLFOLDER -scom -sreg -srd -ag -sfrag -var var.DistDir -out BundleFiles.wxs
        (Get-Content BundleFiles.wxs) -replace '<Component ', '<Component Win64="yes" ' | Set-Content BundleFiles.wxs

        Write-Host "[2/3] Running candle.exe..."
        & "$wixBin\candle.exe" -arch x64 "-dDistDir=$DistDir" -ext "$wixUI" installer\windows\OpenWordPad.wxs BundleFiles.wxs

        Write-Host "[3/3] Running light.exe..."
        & "$wixBin\light.exe" -ext "$wixUI" -sval -b . -out $OutMsi OpenWordPad.wixobj BundleFiles.wixobj
        
        if (Test-Path $OutMsi) {
            Write-Host "MSI generated successfully: $OutMsi ($((Get-Item $OutMsi).Length) bytes)"
        }
    } catch {
        Write-Warning "WiX build failed: $_"
    }
}

# 2. Build Setup.exe with Inno Setup
$iscc = "C:\Program Files (x86)\Inno Setup 6\ISCC.exe"
if (Test-Path $iscc) {
    Write-Host "Compiling Inno Setup installer..."
    & $iscc installer\windows\OpenWordPad.iss
    if (Test-Path "installer\windows\OpenWordPad-Windows-x64-Setup.exe") {
        Move-Item "installer\windows\OpenWordPad-Windows-x64-Setup.exe" "OpenWordPad-Windows-x64-Setup.exe" -Force
        Write-Host "EXE Installer generated successfully: OpenWordPad-Windows-x64-Setup.exe"
    }
}

Write-Host "=== Installer Build Step Finished ==="
