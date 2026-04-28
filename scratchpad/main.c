#include "scratch.h"
#include "sdl-app-init-mini.h"

int main(int argc, char **argv) {
    ecs_world_t *world = NULL;

    if ( SDL_AppInit((void **)&world, argc, argv) != SDL_APP_CONTINUE ) {
        return 1;
    }
    if ( !init_hook(world) ) {;
        return 2;
    }

    bool cont = true;
    while ( cont ) {
        cont = ecs_progress(world, 0.0f) && tick_hook(world);
        SDL_Delay(250);
    }

    fini_hook(world);
    SDL_AppQuit((void **)&world, SDL_APP_SUCCESS);
    SDL_Quit();
    return 0;
}
