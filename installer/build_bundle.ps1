$ErrorActionPreference = "Stop"

$setupProj = Join-Path $PSScriptRoot "Configured.Setup.wixproj"
$bundleProj = Join-Path $PSScriptRoot "Configured.Bundle.wixproj"

Write-Host "=== Build MSI ==="
dotnet build $setupProj -c Release
if ($LASTEXITCODE -ne 0) {
    throw "Failed to build MSI project."
}

Write-Host "=== Build bundle EXE ==="
dotnet build $bundleProj -c Release
if ($LASTEXITCODE -ne 0) {
    throw "Failed to build bundle project."
}

Write-Host "=== Done ==="
