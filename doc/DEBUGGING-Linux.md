
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

I have made a make target for automatically do this with seergdb:
```
cd ~/git/llss/
make -j
make seergdb
```
This will just start, with the correct current directory the following command:
```
SDL_LOGGING="app=trace,assert=warn,test=verbose,*=error" seergdb --bf SDL_AppIterate --cwd . --run program/x64/Debug/llss 
```
You can change the logging string with:
```
make seergdb SDL_LOGGING="*=info"
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

I have made a make target for automatically do those starting steps:
```
cd ~/git/llss/
make renderdoc
```

- llss should display it's main window with renderdoc overlay text saying you can capture with F12
- try make one or two frame capture with F12
- close llss entierly
- you should have a new tab/dock in RenderDoc titled "llss [PID xyz]"
- double-click on one of the frames screenshots to open it
- in the event browser dock, you can click on the clock icon to have duration of each step
- warning: it is a debbugging tool for bad renders and not for bad performance
