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

function Copy-WithStructure {
    param (
        [Parameter(Mandatory)]
        [string[]]$Paths,

        [Parameter(Mandatory)]
        [string]$DestinationRoot
    )

    foreach ($path in $Paths) {
        if (Test-Path $path -PathType Container) {
            # Folders case
            $relative = Resolve-Path $path | ForEach-Object { $_.Path.Substring((Get-Location).Path.Length + 1) }
            Copy-Item $path -Destination (Join-Path $DestinationRoot $relative) -Recurse -Force
        }
        else {
            # Globs or files case
            Get-ChildItem $path -File -ErrorAction SilentlyContinue | ForEach-Object {
                $relative = $_.FullName.Substring((Get-Location).Path.Length + 1)
                $dest = Join-Path $DestinationRoot $relative
                New-Item -ItemType Directory -Path (Split-Path $dest) -Force | Out-Null
                Copy-Item $_.FullName -Destination $dest -Force
            }
        }
    }
}

$metadata_h = "include/metadata.h"

$version = Get-MetadataValue -FilePath $metadata_h -DefineName "APP_VERSION_STR"
$progname = Get-MetadataValue -FilePath $metadata_h -DefineName "APP_METADATA_NAME_STRING"
$prettyos = "Windows"
$artifact = "$progname-$version-$prettyos-$Configuration"

Remove-Item "artifacts/$artifact" -Recurse -Force -ErrorAction SilentlyContinue
New-Item -ItemType Directory -Force "artifacts/$artifact" | Out-Null

$paths = @(
	"doc/user",
	"program/data",
	"program/x64/$Configuration/*.exe"
	"program/x64/$Configuration/*.dll"
)
Get-ChildItem "mods" -Directory | ForEach-Object {
	$paths += "$($_.FullName)/data"
	$paths += "$($_.FullName)/program/x64/$Configuration/*.dll"
}
Copy-WithStructure -Paths $paths -DestinationRoot "artifacts/$artifact"
Write-Output "artifact=$artifact"
