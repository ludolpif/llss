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
#undef ONCE
#ifndef APP_COMPONENTS_MODS_IMPL
#define ECS_META_IMPL EXTERN // Ensure meta symbols are only defined once
#define ONCE extern
#else
#define ONCE
#endif

APP_API void AppComponentsModsImport(ecs_world_t *world);

/* Elements for states represented as exclusive relationships */
APP_API ONCE ECS_TAG_DECLARE(ModState);
APP_API ONCE ECS_ENTITY_DECLARE(ModAvailable);     //  for  ModPrepareFromDisk
APP_API ONCE ECS_ENTITY_DECLARE(ModCopying);       //during ModCopy
APP_API ONCE ECS_ENTITY_DECLARE(ModLoadable);      //  for  ModLoad
APP_API ONCE ECS_ENTITY_DECLARE(ModLoadFailed);    // after ModLoad
APP_API ONCE ECS_ENTITY_DECLARE(ModIncompatible);  // after ModLoad
APP_API ONCE ECS_ENTITY_DECLARE(ModInitializable); //  for  ModInit
APP_API ONCE ECS_ENTITY_DECLARE(ModInitFailed);    // after ModInit
APP_API ONCE ECS_ENTITY_DECLARE(ModRunning);       // after ModInit
APP_API ONCE ECS_ENTITY_DECLARE(ModTerminating);   //  for  ModFini
APP_API ONCE ECS_ENTITY_DECLARE(ModFiniFailed);    // after ModFini
APP_API ONCE ECS_ENTITY_DECLARE(ModUnloadable);    // after ModFini
APP_API ONCE ECS_ENTITY_DECLARE(ModUnloaded);      // after ModUnload

/* Elements for non-exclusive relationships */
APP_API ONCE ECS_TAG_DECLARE(ModFlags);
APP_API ONCE ECS_ENTITY_DECLARE(ModNewerOnDisk);

APP_API ONCE ECS_TAG_DECLARE(ModIOAsync);

/* Component definitions, with metadata for REST API */
ECS_STRUCT(ModOnDisk, {
    char *name;
    char *mod_dirpath;
    char *so_path;
    char *so_realpath;
    SDL_Time modify_time;
    int32_t load_id;
});

typedef struct {
    void *shared_object;
    void *userptr;
    Sint32 build_dep_version_compiled_against;
    SDL_Time modify_time_when_loaded;
    mod_init_v1_t mod_init_v1;
    mod_fini_v1_t mod_fini_v1;
} ModInRAM;
APP_API ONCE ECS_COMPONENT_DECLARE(ModInRAM);

