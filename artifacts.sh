#!/bin/sh
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
		echo artifact=$artifact
	;;
    Debug|Release)
		artifact=$progname-$version-$prettyos-$1
		files="doc/user-manual
	       	program/data
		program/x64/$1/$progname
		mods/*/data mods/*/program/x64/$1/*.so
		"
		ls -d $files | copy_to artifacts/$artifact
		echo artifact=$artifact
	;;
    *) echo "Usage $0 (Sources|Debug|Release)" >&2; exit 1 ;;
esac
exit 0
