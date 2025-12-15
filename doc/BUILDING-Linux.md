# Linux developer and modder environment
I currently use GNU/Linux Debian 13 (trixie) for early development cycles.
It will generate a binary with low system dependancy but needs `GLIBC_2.38` symbols which is not a good choice to distribute currently.

## Prerequisites
- A working system-wide SDL3 shared lib and headers that pkg-config can find.
  - ArchLinux : `pacman -S sdl3`
  - Debian like : `apt install libsdl3-dev`
  - Redhat like : `dnf install libsdl3-devel`
- A GNU make and gcc-based build environnement
  - ArchLinux : `pacman -S base-devel git`
  - Debian like : `apt install build-essential git pkg-config`
  - Redhat like : `dnf groupinstall "Development Tools" && dnf install git`

## Building
I hope provided Makefile are ok on a wide range of linux distro.
Open an issue if you do have building errors.

It's a `git clone` then `./configure` then `make` fashioned system.
Parallel build with `make -j` is way faster but I managed to early commit many Makefile versions that don't really work with `make -j` parallel builds. Hoping this is past.

You can compare with a successufl build trace here: [dev/early/build-trace-linux.txt](dev/early/build-trace-linux.txt)

## Debugging tools
TODO
