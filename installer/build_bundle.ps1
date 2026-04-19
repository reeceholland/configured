$ErrorActionPreference = "Stop"

Write-Host "=== Build MSI ==="
dotnet build .\Configured.Setup.wixproj -c Release

Write-Host "=== Build bundle EXE ==="
dotnet build .\Configured.Bundle.wixproj -c Release

Write-Host "=== Done ==="