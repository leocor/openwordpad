param(
    [string]$DistDir = "dist",
    [string]$OutMsi = "OpenWordPad-Windows-x64-Setup.msi",
    [string]$Version = "1.3.3.0"
)

Write-Host "=== Starting WiX MSI Build ==="
Write-Host "Working Directory: $(Get-Location)"
Write-Host "DistDir contents:"
Get-ChildItem -Path $DistDir | Select-Object Name, Length

# Locate WiX v3 Toolset
$wixBin = ""
if ($env:WIX -and (Test-Path "$env:WIX\bin\heat.exe")) {
    $wixBin = "$env:WIX\bin"
} elseif (Test-Path "C:\Program Files (x86)\WiX Toolset v3.11\bin\heat.exe") {
    $wixBin = "C:\Program Files (x86)\WiX Toolset v3.11\bin"
}

if ($wixBin -ne "") {
    Write-Host "WiX Toolset v3 found at: $wixBin"
    $env:PATH = "$wixBin;$env:PATH"

    Write-Host "[1/3] Running heat.exe to harvest files..."
    & "$wixBin\heat.exe" dir $DistDir -cg AppFiles -dr INSTALLFOLDER -scom -sreg -srd -var var.DistDir -gg -out BundleFiles.wxs
    if ($LASTEXITCODE -ne 0) {
        Write-Error "heat.exe failed with exit code $LASTEXITCODE"
        exit 1
    }

    # Ensure all components have Win64='yes'
    (Get-Content BundleFiles.wxs) -replace '<Component ', '<Component Win64="yes" ' | Set-Content BundleFiles.wxs

    Write-Host "[2/3] Running candle.exe to compile WiX source..."
    & "$wixBin\candle.exe" -arch x64 "-dDistDir=$DistDir" -ext WixUIExtension installer\windows\OpenWordPad.wxs BundleFiles.wxs
    if ($LASTEXITCODE -ne 0) {
        Write-Error "candle.exe failed with exit code $LASTEXITCODE"
        exit 1
    }

    Write-Host "[3/3] Running light.exe to link MSI..."
    & "$wixBin\light.exe" -ext WixUIExtension -sval -b "$DistDir" -b "." -b "resources\icons" -b "installer\windows" -out $OutMsi OpenWordPad.wixobj BundleFiles.wixobj
    if ($LASTEXITCODE -ne 0) {
        Write-Error "light.exe failed with exit code $LASTEXITCODE"
        exit 1
    }

    if (Test-Path $OutMsi) {
        Write-Host "=== MSI Build Successful! ==="
        Write-Host "MSI Path: $OutMsi"
        Write-Host "MSI Size: $((Get-Item $OutMsi).Length) bytes"
        exit 0
    } else {
        Write-Error "MSI file was not produced."
        exit 1
    }
}

Write-Host "WiX v3 not found. Trying WiX v4..."
dotnet tool install --global wix --version 4.0.1 2>$null
wix build installer\windows\OpenWordPad.wxs -d DistDir=$DistDir -o $OutMsi
