# Linux developer and modder environment
I currently use GNU/Linux Debian 13 (trixie) for early development cycles. It have SDL 3.2.10 in main repositories.


## Prerequisites
- A working system-wide SDL3 shared lib and headers that pkg-config can find.
  - ArchLinux : `pacman -S sdl3`
  - Debian like : `apt install libsdl3-dev`
  - Ubuntu 24.04 (LTS) : not available, but we have build them in `llss-build-dep` project
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

TODO: explain details about getting or generating llss-build-dep artifacts in llss git

### Reporting build errors

Open an issue if you do have building errors or newcomer questions.
- If the error occurs using the GUI, please try also MSBuild to attach it's full terminal output in the bug report
You can compare with a successufl build trace here: [dev/early/build-trace-linux.txt](dev/early/build-trace-linux.txt)

## Continuous Integration and Ubuntu LTS

- On Github CI, public linux runner are only based on ubuntu
- We have fully scripted this in `.github/workflow/linux-build.xml`
- If you have Ubuntu 24.04 on your dev computer you can:
```
mkdir -p ~/git
cd ~/git
git clone https://github.com/ludolpif/llss.git`
sudo apt update
./get --build-dep Debug
./get --build-dep SystemLibs
sudo dpkg -i lib/platform/sdl3-deb/libsdl3-*.deb || true; sudo apt install -f
./configure
make -j BUILD_TYPE=${{env.BUILD_CONFIGURATION}}
program/x64/Debug/llss
```

## Debugging

See [DEBUGGING-Linux.md](DEBUGGING-Linux.md) page.
