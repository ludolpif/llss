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

"::notice:: find msbuild.exe"
# MSBuild can came from VS2022 Community edition on a local dev computer, or VS2022 Enterprise on GitHub for exemple.
$msbuild = Get-ChildItem `
    -Path "C:\Program Files\Microsoft Visual Studio\2022\*\MSBuild\Current\Bin\MSBuild.exe" `
    -ErrorAction SilentlyContinue |
    Select-Object -First 1

if (-not $msbuild) {
    [Console]::Error.WriteLine("MSBuild not found.")
    exit 1
}
"::notice msbuild lib.sln '-p:Configuration=$Configuration'"

# powershell "&" is call operator, not unix background task meta-character
& $msbuild.FullName app.sln "-p:Configuration=$Configuration"
