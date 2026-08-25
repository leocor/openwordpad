param(
    [string]$DistDir = "dist",
    [string]$OutMsi = "OpenWordPad-Windows-x64-Setup.msi",
    [string]$Version = "1.3.3.0"
)

Write-Host "=== Starting WiX MSI Build ==="
Write-Host "Current Directory: $(Get-Location)"
Write-Host "DistDir: $DistDir"

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

    Write-Host "[1/3] Harvesting files with heat..."
    & "$wixBin\heat.exe" dir $DistDir -cg AppFiles -dr INSTALLFOLDER -scom -sreg -srd -ag -sfrag -var var.DistDir -out BundleFiles.wxs
    if ($LASTEXITCODE -ne 0) {
        Write-Error "heat.exe failed with exit code $LASTEXITCODE"
        exit 1
    }

    # Ensure all components have Win64='yes'
    (Get-Content BundleFiles.wxs) -replace '<Component ', '<Component Win64="yes" ' | Set-Content BundleFiles.wxs

    Write-Host "[2/3] Compiling WiX sources with candle..."
    & "$wixBin\candle.exe" -arch x64 "-dDistDir=$DistDir" -ext WixUIExtension installer\windows\OpenWordPad.wxs BundleFiles.wxs
    if ($LASTEXITCODE -ne 0) {
        Write-Error "candle.exe failed with exit code $LASTEXITCODE"
        exit 1
    }

    Write-Host "[3/3] Linking MSI with light..."
    & "$wixBin\light.exe" -ext WixUIExtension -sval -out $OutMsi OpenWordPad.wixobj BundleFiles.wixobj
    if ($LASTEXITCODE -ne 0) {
        Write-Error "light.exe failed with exit code $LASTEXITCODE"
        exit 1
    }

    if (Test-Path $OutMsi) {
        Write-Host "=== MSI Build Successful! ==="
        Write-Host "MSI: $OutMsi ($((Get-Item $OutMsi).Length) bytes)"
        exit 0
    }
}

Write-Host "Trying CPack fallback for MSI..."
cpack -G WIX -B cpack_out
Get-ChildItem -Path cpack_out -Filter "*.msi" | Copy-Item -Destination $OutMsi
