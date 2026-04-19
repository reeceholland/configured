param(
    [string]$BuildDir = "..\build",
    [string]$Config = "Release"
)

$ErrorActionPreference = "Stop"

Write-Host "=== Building app ==="
cmake --build $BuildDir --config $Config

$publishDir = Join-Path $PSScriptRoot "publish"

Write-Host "=== Staging app ==="
if (Test-Path $publishDir) {
    Remove-Item $publishDir -Recurse -Force
}
cmake --install $BuildDir --config $Config --prefix $publishDir

Write-Host "=== Deploying Qt runtime ==="
& "C:\Qt\6.6.0\msvc2019_64\bin\windeployqt.exe" (Join-Path $publishDir "configured.exe")

Write-Host "=== Building MSI ==="
dotnet build (Join-Path $PSScriptRoot "Configured.Setup.wixproj") -c Release

Write-Host "=== Done ==="