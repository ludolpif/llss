#pragma once
#include "flecs.h"
#include <SDL3/SDL.h>
void init_hook(ecs_world_t *world);
bool tick_hook(ecs_world_t *world);
void fini_hook(ecs_world_t *world);
