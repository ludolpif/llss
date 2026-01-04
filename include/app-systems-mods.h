#pragma once
#include "app-components-mods.h"

#define APP_MOD_PATH_FROM_BASEPATH "%s../../../mods/"

#ifdef _DEBUG
#define APP_MOD_SUBDIR "program/x64/Debug/"
#else
#define APP_MOD_SUBDIR "program/x64/Release/"
#endif

#if defined(SDL_PLATFORM_WINDOWS)
#define APP_MOD_FILEEXT ".dll"
#elif defined(SDL_PLATFORM_APPLE)
#define APP_MOD_FILEEXT ".dylib"
#else
#define APP_MOD_FILEEXT ".so"
#endif

extern ECS_QUERY_DECLARE(ModReadyQuery);
void AppSystemsModsImport(ecs_world_t *world);
void ModRunInit(ecs_iter_t *it);
