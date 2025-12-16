#!/bin/sh -xe
MAKE="make -j"

name=$(./get --name)
version=$(./get --version)
prettyos=$(./get --prettyos)

artifact_sources=artifacts/$name-$version-Sources

./configure
mkdir -p artifacts
git ls-files > $artifact_sources.lst

rm -f $artifact_sources.zip
zip -qr $artifact_sources.zip . -i@$artifact_sources.lst

for b in Debug Release
do
	artifact_build=artifacts/$name-$version-$prettyos-$b
	$MAKE all BUILD_TYPE=$b
	rm -f $artifact_build.zip
	zip -qr $artifact_build.zip doc/user program/x64/$b/llss mods/*/program/x64/$b/*.so program/data mods/*/data
done
