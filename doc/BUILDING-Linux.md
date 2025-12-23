# Linux developer and modder environment
I currently use GNU/Linux Debian 13 (trixie) for early development cycles. It have SDL 3.2.10 in main repositories.
It will generate a binary with few system dependencies but needs `GLIBC_2.38` symbols which is not a good choice to distribute currently.


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

## Debugging tools

### CPU-side code debbugging

- You can use your favorite GDB frontend (like `ddd` or `seergdb`)
- I didn't tried VSCode but it should work
  - please contribute some instructions for it here if you try
- To have source-display without manual configuration, you need pay attention of current working directoty
  - bad ones : where the executable is, `program/` subfolder
  - good one : the root of llss git repository
- The main entry point typically shown in the fronted before `run` is in SDL headers
  - because we use `#define SDL_MAIN_USE_CALLBACKS` and `#include <SDL3/SDL_main.h>` in `program/main.c`
  - if you have GDB shell, you just can `break SDL_AppIterate` then `run`
  - otherwise your tool should have a "Break in" field where you can give a bare function name like `SDL_AppIterate`

```
cd ~/git/llss/
make -j
ddd program/x64/Debug/llss
(gdb) break SDL_AppIterate
(gdb) run
# main window will appear and hang before drawing the first frame
# use GUI to step/next, add breakpoints, view variables values...
```

### GPU-side render steps debbugging

- On https://renderdoc.org, use the Download (Linux) button
- uncompress the tarbal either in /usr/local or a dedicated folder
```
$ su -
# tar xvf ~youruser/Downloads/renderdoc_1.42.tar.gz -C /usr/local/
```
alternative:
```
$ su -
# mkdir /opt/renderdoc
# tar xvf ~youruser/Downloads/renderdoc_1.42.tar.gz -C /opt/renderdoc/
# exit
$ mkdir ~/bin
$ cd ~/bin
$ ln -s /opt/renderdoc_1.42/bin/qrenderdoc
$ ln -s /opt/renderdoc_1.42/bin/renderdoccmd
```
- install vulkan-validationlayers package
  - ArchLinux : `pacman -S vulkan-validationlayers`
  - Debian like : `apt install vulkan-validationlayers`
  - Redhat like : `dnf install vulkan-validationlayers`
- start renderdoc from a user shell with `qrenderdoc` commmand without arguments
- when the main UI show, hit Ctrl+N to have the "Launch Application" dock/tab in foreground if not already the case
- in "Launch Application" dock/tab
  - Executable path: /home/youruser/git/llss/program/x64/Debug/llss
  - Working directory path: /home/youruser/git/llss/
  - click on "Launch" button
- llss should display it's main window with renderdoc overlay text saying you can capture with F12
- try make one or two frame capture with F12
- close llss entierly
- you should have a new tab/dock in RenderDoc titled "llss [PID xyz]"
- double-click on one of the frames screenshots to open it
- in the event browser dock, you can click on the clock icon to have duration of each step
- warning: it is a debbugging tool for bad renders and not for bad performance
