#pragma once
#include "app.h"

// FLECS Reflection system boilerplate
#undef ECS_META_IMPL
#ifndef MODS_COMPONENTS_CORE_LIFECYCLE_IMPL
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
extern APP_API ECS_ENTITY_DECLARE(ModTerminated);

extern APP_API ECS_TAG_DECLARE(ModFlags);
extern APP_API ECS_ENTITY_DECLARE(ModReloadable);
extern APP_API ECS_ENTITY_DECLARE(ModNewerOnDisk);

//TODO ECS_STRUCT()
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
    SDL_Time so_file_modify_time_when_loaded_in_ram;
    mod_init_v1_t mod_init_v1;
    mod_reload_v1_t mod_reload_v1;
    mod_fini_v1_t mod_fini_v1;
} ModInRAM;
extern APP_API ECS_COMPONENT_DECLARE(ModInRAM);
