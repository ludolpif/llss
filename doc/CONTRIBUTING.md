# How to contribute
This projet is in early stage. Please get in touch before writing too much code.

## Modding possibilities
What programming languages is available for modding ?
- for now, mostly C
- we want to add script language bindings
- FLECS provide a WebSocket API for free, need to mockup things with it

What APIs are available for modding ?
- UI: whole API of Dear ImGuiSDL v1.92.5-docking, through C bindings
  - you can add menu, windows, custom widgets, for the streamer facing UI or in-stream (chatboxes, mini-games...)
  - you can add controls at the bottom of every existing window, menus even if it came from another plugin
- platform and graphics: whole API of SDL 3.2.10
  - rendering is done with `SDL_gpu.h` so we can have mini-games, any kind of shaders
- data: all human-readable data used for render can be altered from a mod through FLECS v4.1.2 API
  - any plugin can add a new Component to existing entities or define new systems that implement a new behavior
- any other libs linked to your mod (needing sane C bindings)
  -  (if you have a big plan, please ask first if there is plans to integrate the lib to the core app )
