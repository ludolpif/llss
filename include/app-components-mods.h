#pragma once
/*
 * This file is part of LLSS.
 *
 * LLSS is free software: you can redistribute it and/or modify it under the terms of the
 * Affero GNU General Public License as published by the Free Software Foundation,
 * either version 3 of the License, or (at your option) any later version.
 *
 * LLSS is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with LLSS.
 * If not, see <https://www.gnu.org/licenses/>. See LICENSE file at root of this git repo.
 *
 * Copyright 2025 ludolpif <ludolpif@gmail.com>
 */
#include "app.h"

// FLECS Reflection system boilerplate
#undef ECS_META_IMPL
#ifndef APP_COMPONENTS_MODS_IMPL
#define ECS_META_IMPL EXTERN // Ensure meta symbols are only defined once
#endif

APP_API void AppComponentsModsImport(ecs_world_t *world);

extern APP_API ECS_TAG_DECLARE(ModState);
extern APP_API ECS_ENTITY_DECLARE(ModAvailable);
extern APP_API ECS_ENTITY_DECLARE(ModIncompatible);
extern APP_API ECS_ENTITY_DECLARE(ModLoadFailed);
extern APP_API ECS_ENTITY_DECLARE(ModReady);
extern APP_API ECS_ENTITY_DECLARE(ModInitFailed);
extern APP_API ECS_ENTITY_DECLARE(ModRunning);
extern APP_API ECS_ENTITY_DECLARE(ModTerminating);
extern APP_API ECS_ENTITY_DECLARE(ModTerminated);

extern APP_API ECS_TAG_DECLARE(ModFlags);
extern APP_API ECS_ENTITY_DECLARE(ModNewerOnDisk);

typedef struct {
    char *name;
    char *so_path;
    SDL_Time modify_time;
} ModOnDisk;
extern APP_API ECS_COMPONENT_DECLARE(ModOnDisk);

typedef struct {
    void *shared_object;
    void *userptr;
    Sint32 build_dep_version_compiled_against;
    SDL_Time modify_time_when_loaded;
    mod_init_v1_t mod_init_v1;
    //XXX mod_reload_v1_t mod_reload_v1;
    mod_fini_v1_t mod_fini_v1;
} ModInRAM;
extern APP_API ECS_COMPONENT_DECLARE(ModInRAM);
