# TODO list

- uniformize naming: ImGui and SDL use camel case for functions, do the same, check what they do on variables names
- mods ABI : put a `_v1` suffix to everything ?
- FLECS
- https://media.bernat.ch/files/debian-debug-packages.pdf
- linux builds compatible with fairly old GLIBC
- early UI
- early releases even it is hello-world like
- imgui config (but find why OBS have leaved Freetype to use native font render on windows)
```
#define IMGUI_DISABLE_STB_TRUETYPE_IMPLEMENTATION
#define IMGUI_ENABLE_FREETYPE
#define IMGUI_ENABLE_FREETYPE_PLUTOSVG
```

## Ideas

- Have a theming color change (blinking ?) when there is network drops

