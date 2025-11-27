#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL_main.h>
// This compilation unit isn't empty because including SDL_main.h include implementation code
//  and define the correct program entrypoint, usually main() but can be WinMain() or so.

// SDL main callbacks implementations are in app-init.c, app-event.c, app-iterate.c
// Global application state is a appstate_t calloc()'ed on heap
//  and SDL callbacks give a userpointer for that (void *_appstate).
