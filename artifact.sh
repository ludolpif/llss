#!/bin/sh
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
copy_to() {
	mkdir -p "$1"
	while read -r path; do
		mkdir -p "$1/$(dirname "$path")"
		cp -r "$path" "$1/$path"
	done
}

progname=$(./get --name)
version=$(./get --version)
prettyos=$(./get --prettyos)

case $1 in
	Sources)
		artifact=$progname-$version-Sources
		git ls-files | copy_to artifacts/$artifact
	;;
	Debug|Release)
		artifact=$progname-$version-$prettyos-$1
		files="doc/user-manual
		program/data
		program/x64/$1/$progname
		mods/*/data mods/*/program/x64/$1/*.so
		"
		ls -d $files | copy_to artifacts/$artifact
	;;
	*) echo "Usage $0 (Sources|Debug|Release)" >&2; exit 1 ;;
esac
echo artifact=$artifact
