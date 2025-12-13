# Mac OS X developer and modder environment
I have no hardware, Makefile are designed to be usable but MacOS specificities aren't really written yet.

## Prerequisites
TODO

### MacOS user session configuration

You will need to unhdide the user `~/Library` special folder in finder:
![Screenshot for unhiding of ~/Library special folder #1](/doc/assets/images/screen-mac-os-finder-unhide-user-library-folder-1.png)
![Screenshot for unhiding of ~/Library special folder #2](/doc/assets/images/screen-mac-os-finder-unhide-user-library-folder-2.png)

### SDL3 development files deployment

- I am not a Apple hardware owner, but I've tried to make some XCode project files manually to help enthousiasts joining me.
- I created a personnal Apple account for code signing for now
- I have followed those things
  - https://github.com/libsdl-org/SDL/blob/main/Xcode/SDL/pkg-support/resources/INSTALL.md
  - and choosen `~/Library/Frameworks` as we may want manual and CLI builds
  - https://journal.stuffwithstuff.com/2025/07/13/setting-up-an-sdl3-mac-app-in-xcode-16/
  - https://wiki.libsdl.org/SDL3/README-macos#setting-up-a-new-project-by-hand



You may have a warning about readlink() failure in `SDL3.xcframework/macos-arm64_x86_64/SDL3.framework/Versions`, if so, it could be the tool used to unzip SDL3 that duplicated the folders instead trying to create a symlink. My quick fix :
```
ludo@Mac-mini-de-Laurent Versions % pwd
/Users/ludo/Library/Frameworks/SDL3.xcframework/macos-arm64_x86_64/SDL3.framework/Versions
ludo@Mac-mini-de-Laurent Versions % ls -al
total 0
drwxr-xr-x  4 ludo  staff  128  8 déc.  15:47 .
drwxr-xr-x  6 ludo  staff  192  8 déc.  15:47 ..
drwxr-xr-x  6 ludo  staff  192  8 déc.  15:47 A
drwxr-xr-x  6 ludo  staff  192  8 déc.  15:47 Current
ludo@Mac-mini-de-Laurent Versions % ls A 
_CodeSignature  Headers         Resources       SDL3
ludo@Mac-mini-de-Laurent Versions % diff -r A Current 
ludo@Mac-mini-de-Laurent Versions % rm -r Current 
ludo@Mac-mini-de-Laurent Versions % ln -s A Current
ludo@Mac-mini-de-Laurent Versions % ls -al
total 0
drwxr-xr-x  4 ludo  staff  128 13 déc.  11:11 .
drwxr-xr-x  6 ludo  staff  192  8 déc.  15:47 ..
drwxr-xr-x  6 ludo  staff  192  8 déc.  15:47 A
lrwxr-xr-x  1 ludo  staff    1 13 déc.  11:11 Current -> A
ludo@Mac-mini-de-Laurent Versions % 
```

## Actually Building
TODO

## Debugging tools
TODO
