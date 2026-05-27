param(
    [Parameter(Mandatory = $true)]
    [string]$Executable,

    [Parameter(ValueFromRemainingArguments = $true)]
    [string[]]$Arguments
)

$ErrorActionPreference = "Stop"

$vswhere = Join-Path ${env:ProgramFiles(x86)} "Microsoft Visual Studio\Installer\vswhere.exe"
if (-not (Test-Path $vswhere)) {
    throw "vswhere.exe was not found. Install Visual Studio 2022 or Build Tools with the C++ workload."
}

$vsInstallPath = & $vswhere -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath
if ([string]::IsNullOrWhiteSpace($vsInstallPath)) {
    throw "No Visual Studio installation with MSVC C++ tools was found."
}

$devShell = Join-Path $vsInstallPath "Common7\Tools\Launch-VsDevShell.ps1"
if (-not (Test-Path $devShell)) {
    throw "Launch-VsDevShell.ps1 was not found under $vsInstallPath."
}

& $devShell -Arch amd64 -SkipAutomaticLocation
if ($LASTEXITCODE -ne 0) {
    exit $LASTEXITCODE
}

& $Executable @Arguments
if ($LASTEXITCODE -ne 0) {
    exit $LASTEXITCODE
}
