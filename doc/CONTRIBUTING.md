# How to contribute
This projet is in early stage. Please get in touch before writing too much code.

## Modding possibilities
What programming languages is available for modding?
- for now, mostly C and FLECS scripts
- we want to add other script language bindings
- FLECS provide a WebSocket API for free, need to mockup things with it
  - for now, https://www.flecs.dev/explorer/?host=localhost

What APIs are available for modding?
- UI: whole API of Dear ImGuiSDL v1.92.5-docking, through C bindings
  - you can add menu, windows, custom widgets, for the streamer facing UI or in-stream (chatboxes, mini-games...)
  - you can add controls at the bottom of every existing window, menus even if it came from another plugin
- platform and graphics: whole API of SDL 3.2.10
  - rendering is done with `SDL_gpu.h` so we can have mini-games, any kind of shaders
- data: all human-readable data used for render can be altered from a mod through FLECS v4.1.4 API
  - any plugin can add a new Component to existing entities or define new systems that implement a new behavior
- any other libs linked to your mod (needing sane C bindings)
  -  (if you have a big plan, please ask first if there is plans to integrate the lib to the core app )
- read the whole `include/api.h` file, there is many comments there

## What are the difficult/ambiguous terms to know?

- mod: as in Minecraft or any maddable game. For this app, it's a dynamic hot-reloadable C plugin
- module: is a FLECS module, is a C compilation unit, implements an import function usable by `ECS_IMPORT()`

## How it works at a coarse level?

- `main()` entrypoint is in `program/main.c` compilation unit but not written by us, but by SDL3 authors
  - they take care of OS specificities (for MacOS, be a Cocoa application, for Windows define `WinMain()`...)
- SDL3 `main()` will do this in a portable and error-checked manner:
```c
int main(...) {
    enum SDL_AppResult res;             // https://wiki.libsdl.org/SDL3/SDL_AppResult
    SDL_RunApp(...);                    // SDL3 defined, initialize platform specific things
    res = SDL_AppInit();                // program defined, in sdl-app-init.c
    while ( res == SDL_APP_CONTINUE ) {
        while ( SDL_Event event = pop(user_input_event_list) ) {
            res = SDL_AppEvent(event);  // program defined, in sdl-app-event.c
        }
        res = SDL_App_Iterate();        // program defined, in sdl-app-iterate.c
        // No IPS/FPS throttling
    }
    SDL_AppQuit();                      // program defined, in sdl-app-init.c
    return res;
}
```
- data is stored in plain C structs but we give many informations to FLECS to read/write/serialize
  - FLECS provide macros and functions to do that. See `include/app-components-*.h`.
- our `SDL_AppInit()` will init many little things, but the big ones are:
```c
SDL_AppResult SDL_AppInit(...) {
    SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO | SDL_INIT_GAMEPAD | SDL_INIT_CAMERA);
    SDL_Window        *main_window = SDL_CreateWindow(...);
    SDL_GPUDevice      *gpu_device = SDL_CreateGPUDevice(...); // SDLGPU backend (not OpenGL)
    SDL_AsyncIOQueue *sdl_io_queue = SDL_CreateAsyncIOQueue();

    ImGui_CreateContext(...);
    cImGui_ImplSDL3_InitForSDLGPU(main_window);

    ecs_world_t *world = ecs_init();    // FLECS init and modules loading
    ECS_IMPORT(world, AppSystemsCore);
    ECS_IMPORT(world, AppSystemsMods);
    // Note: AppSystems* FLECS modules depends on components modules, so everything is loaded

    return SDL_APP_CONTINUE;
}
```
- now we have an initalized SDLGPU / ImGui / FLECS based-app, we need each video frame rendering
  - this is not a simplified version, every is there, the ECS is central
```c
SDL_AppResult SDL_AppIterate(void *appstate) {
    ecs_world_t *world = (ecs_world_t *)appstate;

    // Make most of the work from ECS pipeline as it allow dynamic scheduling
    // without recompiling this program
    ecs_progress(world, 0.0f);

    // Mods init and reload is defered outside of ecs_progress() so they can ECS_IMPORT() modules
    // while the world isn't in read-only mode. mod fini needs it too.
    ecs_iter_t it = ecs_query_iter(world, ModRunningNewerOnDiskQuery);
    while (ecs_query_next(&it)) {
        ModFini(&it);
    }
    it = ecs_query_iter(world, ModTerminatingQuery);
    while (ecs_query_next(&it)) {
        ModFini(&it);
    }
    it = ecs_query_iter(world, ModInitializableQuery);
    while (ecs_query_next(&it)) {
        ModInit(&it);
    }

    // Custom method for targetting desired FPS
    throttle(world);

    // TODO trigger QUIT here depending on an info from the ECS
    return SDL_APP_CONTINUE;
}
```
- mods will auto-load after the first rendered frames because ECS\_IMPORTed systems define this behavor
  - you can watch your mod state using FLECS Explorer
  - the mod hot-reloading is implemented in `program/app-systems-mods.c`

![Screenshot of FLECS explorer for mod state checking](/doc/assets/images/screen-flecs-explorer-mod-state.png)

- mods can (should) define new FLECS modules to:
  - make their data avaiable to the app, API and all others mods
  - implement some new behaviors and functionalities as FLECS Systems, to be scheduled by FLECS
  - ECS is not using the usual Object-Oriented way of thinking and it's overwhelming to "unlearn" at first try
  - Official FLECS newcomer documentation is great, but it takes a for weeks to be good at it
  - https://www.flecs.dev/flecs/#what-is-an-entity-component-system

- FLECS 
  - use many conventions and you need to know them (some use `PascalCase`, some `snaked_case`)
  - translate modules names like `ModScratchpadCompositing` and create namespaced entities in `mod.scratchpad.compositing` tree.
  - suggests to make some `*.components.*` modules for data and separate `*.systems.*`
  - we suggest that all original program go in `app.*` and all mods FLECS modules go in `mod.*`
  - So you should use `ModSomething` names to be in `mod.*` entity subtree
