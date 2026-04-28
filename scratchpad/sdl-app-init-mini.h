#pragma once
#include "app.h"
SDL_AppResult SDL_AppInit(void **appstate, int argc, char **argv);
void SDL_AppQuit(void *appstate, SDL_AppResult result);
