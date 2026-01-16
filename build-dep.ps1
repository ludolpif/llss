#
# This file is part of LLSS.
#
# LLSS is free software: you can redistribute it and/or modify it under the terms of the
# Affero GNU General Public License as published by the Free Software Foundation,
# either version 3 of the License, or (at your option) any later version.
#
# LLSS is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
# without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License along with LLSS.
# If not, see <https://www.gnu.org/licenses/>. See LICENSE file at root of this git repo.
#
# Copyright 2025 ludolpif <ludolpif@gmail.com>
#
param (
    [Parameter(Mandatory = $false)]
    [string]$Configuration
)

if ($Configuration -ne "Debug" -and $Configuration -ne "Release") {
    [Console]::Error.WriteLine("Usage: $PSCommandPath (Debug|Release)")
    exit 1
}

function Get-MetadataValue {
    param (
        [Parameter(Mandatory)]
        [string]$FilePath,

        [Parameter(Mandatory)]
        [string]$DefineName
    )

    $pattern = "^\s*#define\s+$DefineName\s+""([^""]+)"""
    $match = Select-String -Path $FilePath -Pattern $pattern

    if (-not $match) {
        throw "Can't find $DefineName in $FilePath"
    }

    return $match.Matches[0].Groups[1].Value
}

$build_dep_version = Get-MetadataValue -FilePath "include/version-build-dep.h" -DefineName "BUILD_DEP_VERSION_STR"
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
