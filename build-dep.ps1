param (
    [Parameter(Mandatory = $false)]
    [string]$Configuration
)

if ($Configuration -ne "Debug" -and $Configuration -ne "Release") {
    [Console]::Error.WriteLine("Usage: $PSCommandPath (Debug|Release)")
    exit 1
}

$build_dep_version = "0.2.0.0"
$build_dep_zip = "build-dep-${build_dep_version}-Windows-$Configuration.zip"
$build_dep_url = "https://ludolpif.fr/pub/llss/artifacts/$build_dep_zip"

"::notice:: re-use or download '$build_dep_zip'"
if (-Not (Test-Path "$build_dep_zip")) {
    Invoke-WebRequest -Uri $build_dep_url -OutFile "$build_dep_zip"
}
"::notice:: unzip '$build_dep_zip'"
Expand-Archive -Path "$build_dep_zip" -DestinationPath "." -Force

# Remove-Item -Path "$build_dep_zip"
"::group::lib\platform content at end of script"
Get-ChildItem -Force # ls -a equivalent
"::endgroup::"
