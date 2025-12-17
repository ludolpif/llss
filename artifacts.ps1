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

# Chemin vers metadata.h
$metadata_h = "include/metadata.h"

# Extraction des valeurs
$version = Get-MetadataValue -FilePath $metadata_h -DefineName "APP_VERSION_STR"
$name    = Get-MetadataValue -FilePath $metadata_h -DefineName "APP_METADATA_NAME_STRING"
$prettyos  = "Windows"
$target = "artifacts/$name-$version-$prettyos-$Configuration"

Remove-Item $target -Recurse -Force -ErrorAction SilentlyContinue
New-Item -ItemType Directory -Force $target | Out-Null

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
Copy-WithStructure -Paths $paths -DestinationRoot $target
