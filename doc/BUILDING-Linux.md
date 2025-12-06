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

Below a trace for reference:
```
ludolpif@lud-5490:~/git$ git clone https://github.com/ludolpif/llss.git llss
Cloning into 'llss'...
remote: Enumerating objects: 534, done.
remote: Counting objects: 100% (110/110), done.
remote: Compressing objects: 100% (66/66), done.
remote: Total 534 (delta 50), reused 83 (delta 35), pack-reused 424 (from 1)
Receiving objects: 100% (534/534), 1.13 MiB | 8.44 MiB/s, done.
Resolving deltas: 100% (246/246), done.
ludolpif@lud-5490:~/git$ cd llss
ludolpif@lud-5490:~/git/llss$ ./configure 
+ git                                                                      FOUND
+ make                                                                     FOUND
+ gcc                                                                      FOUND
+ ar                                                                       FOUND
+ pkg-config                                                               FOUND
+ pkg-config sdl3 --cflags
+ git submodule update --init --recursive lib/ui/imgui
Submodule 'dear_imgui' (https://github.com/ludolpif/dear_imgui.git) registered for path 'lib/ui/imgui'
Cloning into '/home/ludolpif/git/llss/lib/ui/imgui'...
Submodule path 'lib/ui/imgui': checked out '79a89a40e96554922f6530a37e5a517faaf6b9ed'
ludolpif@lud-5490:~/git/llss$ make -j
make -C src
make -C mods/llss-mod-template/src
make[1]: Entering directory '/home/ludolpif/git/llss/mods/llss-mod-template/src'
make[1]: Entering directory '/home/ludolpif/git/llss/src'
make -C ../lib/ui
make -C ../lib/ecs
make[2]: Entering directory '/home/ludolpif/git/llss/lib/ui'
g++ -g -O3 `./cflags`   -c -o imgui.o imgui/imgui.cpp
make[2]: Entering directory '/home/ludolpif/git/llss/lib/ecs'
cc -g -O3 -std=gnu99 -Iflecs -c -o flecs.o flecs/flecs.c
g++ -g -O3 `./cflags`   -c -o imgui_demo.o imgui/imgui_demo.cpp
make -C ../../../lib/ui
g++ -g -O3 `./cflags`   -c -o imgui_draw.o imgui/imgui_draw.cpp
make -C ../../../lib/ecs
g++ -g -O3 `./cflags`   -c -o imgui_tables.o imgui/imgui_tables.cpp
make[2]: Entering directory '/home/ludolpif/git/llss/lib/ecs'
cc -g -O3 -std=gnu99 -Iflecs -c -o flecs.o flecs/flecs.c
make[2]: Entering directory '/home/ludolpif/git/llss/lib/ui'
g++ -g -O3 `./cflags`   -c -o imgui.o imgui/imgui.cpp
g++ -g -O3 `./cflags`   -c -o imgui_widgets.o imgui/imgui_widgets.cpp
g++ -g -O3 `./cflags`   -c -o imgui_demo.o imgui/imgui_demo.cpp
g++ -g -O3 `./cflags`   -c -o imgui_impl_sdl3.o imgui/backends/imgui_impl_sdl3.cpp
g++ -g -O3 `./cflags`   -c -o imgui_draw.o imgui/imgui_draw.cpp
g++ -g -O3 `./cflags`   -c -o imgui_tables.o imgui/imgui_tables.cpp
g++ -g -O3 `./cflags`   -c -o imgui_widgets.o imgui/imgui_widgets.cpp
g++ -g -O3 `./cflags`   -c -o imgui_impl_sdlgpu3.o imgui/backends/imgui_impl_sdlgpu3.cpp
g++ -g -O3 `./cflags`   -c -o imgui_impl_sdl3.o imgui/backends/imgui_impl_sdl3.cpp
g++ -g -O3 `./cflags`   -c -o dcimgui.o dear_bindings_generated/dcimgui.cpp
g++ -g -O3 `./cflags`   -c -o imgui_impl_sdlgpu3.o imgui/backends/imgui_impl_sdlgpu3.cpp
g++ -g -O3 `./cflags`   -c -o dcimgui_impl_sdl3.o dear_bindings_generated/backends/dcimgui_impl_sdl3.cpp
g++ -g -O3 `./cflags`   -c -o dcimgui.o dear_bindings_generated/dcimgui.cpp
g++ -g -O3 `./cflags`   -c -o dcimgui_impl_sdl3.o dear_bindings_generated/backends/dcimgui_impl_sdl3.cpp
g++ -g -O3 `./cflags`   -c -o dcimgui_impl_sdlgpu3.o dear_bindings_generated/backends/dcimgui_impl_sdlgpu3.cpp
g++ -g -O3 `./cflags`   -c -o dcimgui_impl_sdlgpu3.o dear_bindings_generated/backends/dcimgui_impl_sdlgpu3.cpp
ar rcs libdcimgui.a imgui.o imgui_demo.o imgui_draw.o imgui_tables.o imgui_widgets.o imgui_impl_sdl3.o imgui_impl_sdlgpu3.o dcimgui.o dcimgui_impl_sdl3.o dcimgui_impl_sdlgpu3.o
make[2]: Leaving directory '/home/ludolpif/git/llss/lib/ui'
ar rcs libdcimgui.a imgui.o imgui_demo.o imgui_draw.o imgui_tables.o imgui_widgets.o imgui_impl_sdl3.o imgui_impl_sdlgpu3.o dcimgui.o dcimgui_impl_sdl3.o dcimgui_impl_sdlgpu3.o
make[2]: Leaving directory '/home/ludolpif/git/llss/lib/ui'
make[2]: Leaving directory '/home/ludolpif/git/llss/lib/ecs'
gcc -MT alloc.o -MMD -MP -MF .deps/alloc.d -g -Wall -Wformat -O0  `./cflags`   -c -o alloc.o alloc.c
gcc -MT app-event.o -MMD -MP -MF .deps/app-event.d -g -Wall -Wformat -O0  `./cflags`   -c -o app-event.o app-event.c
gcc -MT app-init.o -MMD -MP -MF .deps/app-init.d -g -Wall -Wformat -O0  `./cflags`   -c -o app-init.o app-init.c
gcc -MT app-iterate.o -MMD -MP -MF .deps/app-iterate.d -g -Wall -Wformat -O0  `./cflags`   -c -o app-iterate.o app-iterate.c
gcc -MT main.o -MMD -MP -MF .deps/main.d -g -Wall -Wformat -O0  `./cflags`   -c -o main.o main.c
gcc -MT mod-host.o -MMD -MP -MF .deps/mod-host.d -g -Wall -Wformat -O0  `./cflags`   -c -o mod-host.o mod-host.c
gcc -MT ui-main.o -MMD -MP -MF .deps/ui-main.d -g -Wall -Wformat -O0  `./cflags`   -c -o ui-main.o ui-main.c
gcc -g -Wall -Wformat -O0  `./cflags`  -Wl,-Bsymbolic -L../lib/ui  alloc.o app-event.o app-init.o app-iterate.o main.o mod-host.o ui-main.o ../lib/ecs/flecs.o -ldcimgui -lm -lstdc++ -lSDL3 -o ../app
make[1]: Leaving directory '/home/ludolpif/git/llss/src'
make[2]: Leaving directory '/home/ludolpif/git/llss/lib/ecs'
gcc -MT hello.o -MMD -MP -MF .deps/hello.d -g -Wall -Wformat -O0 `./cflags`   -c -o hello.o hello.c
gcc -MT mod.o -MMD -MP -MF .deps/mod.d -g -Wall -Wformat -O0 `./cflags`   -c -o mod.o mod.c
gcc -g -Wall -Wformat -O0 `./cflags`  -Wl,-Bsymbolic -Wl,--default-symver -Wl,-soname,llss-mod-template.so -shared -L../../../lib/ui  hello.o mod.o -ldcimgui -lm -lstdc++ -o ../llss-mod-template.so
make[1]: Leaving directory '/home/ludolpif/git/llss/mods/llss-mod-template/src'
ludolpif@lud-5490:~/git/llss$ ./app 
INFO  0000000000000055 SDL_AppInit(): tick0_wallclock==1764707268650884098
INFO  0000000261642776 ECS world initialized, first entity name: Bob
WARN  0000000261702845 mod_tryload(): will load /home/ludolpif/git/llss/mods/mod-template/mod-template.so
INFO  0000000261990450 heap allocation at end of SDL_AppInit:
INFO  [      0 frames] ctxt   malloc   calloc  realloc     free
INFO       STARTUP_SHUTDOWN     1810      324       45      474 (+1660)
INFO           FIRST_FRAMES        0        0        0        0 (+0)
INFO              RENDERING        0        0        0        0 (+0)
INFO  This only count calls via SDL_(*alloc|free)
INFO  0000008492174162 heap allocation at end of SDL_AppQuit:
INFO  [    494 frames] ctxt   malloc   calloc  realloc     free
INFO       STARTUP_SHUTDOWN     1855      324       47      993 (+1186)
INFO           FIRST_FRAMES      589       62       49      452 (+199)
INFO              RENDERING      271       97       26      349 (+19)
INFO  This only count calls via SDL_(*alloc|free)
ludolpif@lud-5490:~/git/llss$ 
```

## Debugging tools
TODO
