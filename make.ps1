param (
    [Parameter(Mandatory = $false)]
    [string]$Configuration
)

if ($Configuration -ne "Debug" -and $Configuration -ne "Release") {
    [Console]::Error.WriteLine("Usage: $PSCommandPath (Debug|Release)")
    exit 1
}

# MSBuild vient de VS2022 Community sur poste de dev et de VS2022 Enterprise sur GitHub CI par exemple.
$msbuild = Get-ChildItem `
    -Path "C:\Program Files\Microsoft Visual Studio\2022\*\MSBuild\Current\Bin\MSBuild.exe" `
    -ErrorAction SilentlyContinue |
    Select-Object -First 1

if (-not $msbuild) {
    [Console]::Error.WriteLine("MSBuild introuvable.")
    exit 1
}

# En powershell "&" est le call operator, different de unix (execution en arriere plan si en fin de ligne)
& $msbuild.FullName app.sln "-p:Configuration=$Configuration"
& $msbuild.FullName mod-template.sln "-p:Configuration=$Configuration"
