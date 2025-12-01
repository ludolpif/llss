# TODO list

- plugin system (SDL have LoadLibrary)
  - https://www.akkadia.org/drepper/dsohowto.pdf from https://gcc.gnu.org/wiki/Visibility
  - https://learn.microsoft.com/en-us/windows/win32/api/libloaderapi/nf-libloaderapi-loadlibraryw
  - https://github.com/libsdl-org/SDL/blob/main/src/loadso/windows/SDL_sysloadso.c
  - https://github.com/libsdl-org/SDL/blob/main/src/loadso/dlopen/SDL_sysloadso.c
- FLECS
- https://media.bernat.ch/files/debian-debug-packages.pdf

```c
https://github.com/ocornut/imgui/blob/master/imgui.cpp
// DLL users:
// - Heaps and globals are not shared across DLL boundaries!
// - You will need to call SetCurrentContext() + SetAllocatorFunctions() for each static/DLL boundary you are calling from.
// - Important: Dear ImGui functions are not thread-safe because of this pointer.
//   - Future development aims to make this context pointer explicit to all calls. Also read https://github.com/ocornut/imgui/issues/586
```
