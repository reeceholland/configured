param(
    [string]$BuildDir = (Join-Path $PSScriptRoot "..\out\build\x64-release"),
    [string]$Config = "Release",
    [string]$QtBinDir = "C:\Qt\6.10.1\msvc2022_64\bin"
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
& (Join-Path $QtBinDir "windeployqt.exe") (Join-Path $publishDir "Configured.exe")

Write-Host "=== Building MSI ==="
dotnet build (Join-Path $PSScriptRoot "Configured.Setup.wixproj") -c Release

Write-Host "=== Done ==="
