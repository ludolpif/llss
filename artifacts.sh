#!/bin/sh
MAKE="make -j"

name=$(./get --name)
version=$(./get --version)
prettyos=$(./get --prettyos)

mkarchive_sources() {
	target=artifacts/$name-$version-Sources
	set -xe
	git ls-files > $target.lst
	rm -f $target.zip
	zip -r $target.zip . -i@$target.lst
	exit $?
}

mkarchive_bin() {
	target=artifacts/$name-$version-$prettyos-$1
	set -xe
	rm -f $target.zip
	zip -r $target.zip doc/user program/x64/$1/llss mods/*/program/x64/$1/*.so program/data mods/*/data
	exit $?
}

case $1 in
	Sources) mkdir -p artifacts && mkarchive_sources ;;
	Debug)   mkdir -p artifacts && mkarchive_bin $1 ;;
	Release) mkdir -p artifacts && mkarchive_bin $1 ;;
	*) echo "Usage $0 (Sources|Debug|Release)" >&2; exit 1;;
esac
