# TODO list
- change licence to Affero for --headless, add it to all files
- uniformize naming: ImGui and SDL use camel case for functions, do the same, check what they do on variables names
- mods ABI : put a `_v1` suffix to everything ?
- FLECS
- lygia and `SDL_shadercompiler`
- early switch to alpha blending in linear space
- https://media.bernat.ch/files/debian-debug-packages.pdf
- linux builds compatible with fairly old GLIBC
- early UI with menu, 
- early releases even it is hello-world like
- imgui config (but find why OBS have leaved Freetype to use native font render on windows)
```
See imgui/misc/imgui_freetype.cpp to make it
#define IMGUI_DISABLE_STB_TRUETYPE_IMPLEMENTATION
#define IMGUI_ENABLE_FREETYPE
#define IMGUI_ENABLE_FREETYPE_PLUTOSVG
```

## Ideas

- Have a theming color change (blinking ?) when there is network drops
- Have getopt for --headless --profile --safe-mode (plugin to be enabled by hand clicking checkboxes in a window)
- Have a script mod which run in headless (or not) that enable local record with a choosen name, demo-ing filters and transitions
- Try use ImGui layers and try to reder to texture intermediate steps. If works, allow gizmo on  preview but not on program
- Source compositing may be based on ImGui widgets (a source like an undecorated window by default
- ImGui Theme may apply differently for some windows ? if so, welcome to in-stream 9-slice window decoration and so
- imgui window in-stream mini-game / twtichplays or streamer playing from a mod
- all window can go in-stream at any time with a checkbox in window header ?
- twich chat in-stream window as a mod ? performance will be there ! auto-scroll-pausable, mouse control for streamer both for him or in-stream
