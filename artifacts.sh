#!/bin/sh
copy_to() {
    mkdir -p "$1"
    while read -r path; do
		mkdir -p "$1/$(dirname "$path")"
		cp -r "$path" "$1/$path"
    done
}

name=$(./get --name)
version=$(./get --version)
prettyos=$(./get --prettyos)

case $1 in
    Sources)
		target=artifacts/$name-$version-Sources
		git ls-files | copy_to "$target"
	;;
    Debug|Release)
		target=artifacts/$name-$version-$prettyos-$1
		ls doc/user program/data program/x64/$1/$name mods/*/data mods/*/program/x64/$1/*.so | copy_to "$target"
	;;
    *) echo "Usage $0 (Sources|Debug|Release)" >&2; exit 1 ;;
esac
exit 0
