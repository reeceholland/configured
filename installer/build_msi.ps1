param(
    [string]$SourceDir = (Resolve-Path (Join-Path $PSScriptRoot "..")).Path,
    [string]$BuildDir = (Join-Path $PSScriptRoot "..\out\build\x64-release"),
    [string]$Config = "Release",
    [string]$QtBinDir = "C:\Qt\6.10.1\msvc2022_64\bin",
    [string]$VcpkgRoot = $env:VCPKG_INSTALLATION_ROOT,
    [string]$VcpkgTriplet = "x64-windows"
)

$ErrorActionPreference = "Stop"

if ([string]::IsNullOrWhiteSpace($VcpkgRoot) -and (Test-Path "C:\dev\vcpkg")) {
    $VcpkgRoot = "C:\dev\vcpkg"
}

Write-Host "=== Configuring release build ==="
$configureArgs = @(
    "-S", $SourceDir,
    "-B", $BuildDir,
    "-G", "Ninja",
    "-DCMAKE_BUILD_TYPE=$Config",
    "-DBUILD_TESTING=OFF"
)

if (-not [string]::IsNullOrWhiteSpace($VcpkgRoot)) {
    $configureArgs += "-DCMAKE_TOOLCHAIN_FILE=$(Join-Path $VcpkgRoot "scripts\buildsystems\vcpkg.cmake")"
}

cmake @configureArgs

Write-Host "=== Building app ==="
cmake --build $BuildDir --config $Config --target Configured

$publishDir = Join-Path $PSScriptRoot "publish"

Write-Host "=== Staging app ==="
if (Test-Path $publishDir) {
    Remove-Item $publishDir -Recurse -Force
}
cmake --install $BuildDir --config $Config --prefix $publishDir

Write-Host "=== Deploying Qt runtime ==="
& (Join-Path $QtBinDir "windeployqt.exe") (Join-Path $publishDir "Configured.exe")

if (-not [string]::IsNullOrWhiteSpace($VcpkgRoot)) {
    Write-Host "=== Deploying yaml-cpp runtime ==="
    $vcpkgBinDir = Join-Path $VcpkgRoot "installed\$VcpkgTriplet\bin"
    $yamlDlls = Get-ChildItem -Path $vcpkgBinDir -Filter "yaml-cpp*.dll" -ErrorAction SilentlyContinue

    if (-not $yamlDlls) {
        throw "Could not find yaml-cpp runtime DLLs in $vcpkgBinDir."
    }

    foreach ($dll in $yamlDlls) {
        Copy-Item -LiteralPath $dll.FullName -Destination $publishDir -Force
    }
}

Write-Host "=== Building MSI ==="
dotnet build (Join-Path $PSScriptRoot "Configured.Setup.wixproj") -c Release

Write-Host "=== Done ==="
