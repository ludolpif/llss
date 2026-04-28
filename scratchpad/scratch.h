#pragma once
#include "app.h"
//#include "flecs.h"
//#include <SDL3/SDL.h>
bool init_hook(ecs_world_t *world);
bool tick_hook(ecs_world_t *world);
void fini_hook(ecs_world_t *world);
