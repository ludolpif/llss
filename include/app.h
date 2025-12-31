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
 *
 * This file is our main program public API. mods can #include it and use everything.
 * Kept in one file to help searching... but long, so quick table of content.
 * Note: flecs.h and dcimgui.h do the same, check them too. For SDL3, check this link:
 * https://wiki.libsdl.org/SDL3/CategoryAPI
 *
 * [SECTION] Conventions
 * [SECTION] Libraries directly usable from mods
 * [SECTION] General and metadata macros definitions
 * [SECTION] Logging helpers and counters definitions
 * [SECTION] Structures for global-like plain old data
 * [SECTION] Mods API definition
 * [SECTION] ECS core components definitions
 */

//-----------------------------------------------------------------------------
// [SECTION] Conventions
//-----------------------------------------------------------------------------

/* Choosen naming conventions here = same as FLECS (ImGui and SDL have different ones)
 * Full details: https://www.flecs.dev/flecs/md_docs_2Manual.html#naming-conventions
 *
 * - Configuration and constants macro use SCREAMING_SNAKE_CASE
 * - Declarative function-style macros use SCREAMING_SNAKE_CASE()
 * - Function wrapper macros like app_info() use snake_case()
 * - Regular C functions use snake_case()
 * - API types use snake_case_t
 * - struct that are used as ECS Component names (like 'Position') use PascalCase
 * - functions that are used as ECS System (like 'Move') use PascalCase
 */

//-----------------------------------------------------------------------------
// [SECTION] Libraries directly usable from mods
//-----------------------------------------------------------------------------
#include <SDL3/SDL.h>
//
//TODO consider #define IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_USER_CONFIG "imgui_config.h"
#include "dcimgui.h"

//TODO in build-dep first
//#define ecs_ftime_t double //Change to double precision for processes that can run for a long time (e.g. longer than a day).
#include "flecs.h"

//-----------------------------------------------------------------------------
// [SECTION] General and metadata macro definitions
//-----------------------------------------------------------------------------
#include "metadata.h"
// Following macros can't be in metadata.h as Microsoft res.exe can't cope with it (app.rc includes metadata.h)
#define VERSION_TO_INT(a, b, c) ((a)<<16 | (b)<<8 | (c))
#define	VERSION_MAJOR_FROM_INT(a) ((a) >> 16)
#define	VERSION_MINOR_FROM_INT(a) (((a) & 0x00FF00) >> 8)
#define	VERSION_MICRO_FROM_INT(a) ((a) & 0xFF)

#define APP_VERSION_INT VERSION_TO_INT(APP_VERSION_MAJOR,APP_VERSION_MINOR,APP_VERSION_PATCH)
#define BUILD_DEP_VERSION_INT VERSION_TO_INT(BUILD_DEP_VERSION_MAJOR,BUILD_DEP_VERSION_MINOR,BUILD_DEP_VERSION_PATCH)

//-----------------------------------------------------------------------------
// [SECTION] Logging helpers and counters definitions
//-----------------------------------------------------------------------------
/* TODO
typedef enum app_logcategory {
	APP_CATEGORY_CORE = SDL_LOG_CATEGORY_CUSTOM,
	APP_CATEGORY_PLUGIN,
} app_logcategory_t;
*/

// Convention: do not use app_info(), app_warn() for messages that can happen at each frame and flood the log, use app_debug() or counters
#define app_trace(...)    if (logpriority_earlyskip <= SDL_LOG_PRIORITY_TRACE) SDL_LogTrace(SDL_LOG_CATEGORY_APPLICATION, __VA_ARGS__)
#define app_verbose(...)  if (logpriority_earlyskip <= SDL_LOG_PRIORITY_VERBOSE) SDL_LogVerbose(SDL_LOG_CATEGORY_APPLICATION, __VA_ARGS__)
#define app_debug(...)    if (logpriority_earlyskip <= SDL_LOG_PRIORITY_DEBUG) SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, __VA_ARGS__)
#define app_info(...)     SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, __VA_ARGS__)
#define app_warn(...)     SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, __VA_ARGS__)
#define app_error(...)    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, __VA_ARGS__)
#define app_critical(...) SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, __VA_ARGS__)

//-----------------------------------------------------------------------------
// [SECTION] Structures for global-like plain old data
//-----------------------------------------------------------------------------
extern SDL_LogPriority logpriority_earlyskip;

//-----------------------------------------------------------------------------
// [SECTION] Mods API definition
//-----------------------------------------------------------------------------
typedef enum mod_result {
	MOD_RESULT_INVALID,
	MOD_RESULT_FAILURE,
	MOD_RESULT_SUCCESS,
	MOD_RESULT_CONTINUE
} mod_result_t;
/**
 * @symbol-name    mod_handshake_v1
 * @calling-thread SDL_Main
 * @called-from    program/mod-host.c
 * @when           after SDL_LoadObject(".../this-mod.so")
 * @mandatory      yes
 * @purpose        version compatibility check while a mod is loaded. no data availble yet.
 * @definition     SDL_DECLSPEC Sint32 SDLCALL mod_handshake(Sint32 running_app_version) { ... }
 */
typedef Sint32 (*mod_handshake_v1_t)(Sint32 running_app_version);

/**
 * @symbol-name    mod_init_v1
 * @calling-thread SDL_Main
 * @called-from    program/mod-host.c
 * @when           after mod_handshake_v1 success
 * @mandatory      yes
 * @purpose        mod own state initialization
 * @param world    in: main app ECS world, this pointer value should be copied in mod main data struct
 * @param userptr  out: userptr that be passed when future hooks will be called, should be SDL_calloc()ed by the mod
 * @definition     SDL_DECLSPEC mod_result_t SDLCALL mod_init_v1(ecs_world_t *world, void **userptr) { ... }
 */
typedef mod_result_t (*mod_init_v1_t)(ecs_world_t *world, void **userptr);

/**
 * @symbol-name    mod_fini_v1
 * @calling-thread SDL_Main
 * @called-from    program/mod-host.c
 * @when           right before SDL_UnloadObject()
 * @mandatory      yes
 * @purpose        mod own state de-initialization (free structs from stack)
 * @definition     SDL_DECLSPEC mod_result_t SDLCALL mod_fini_v1(void *userptr) { ... }
 */
typedef mod_result_t (*mod_fini_v1_t)(void *userptr);

/**
 * @symbol-name    mod_reload_v1
 * @calling-thread SDL_Main
 * @called-from    program/mod-host.c
 * @when           after loading the new one, before unloading the old one
 * @mandatory      yes
 * @purpose        allow mod hot-reloading by given the new one a pointer to the data of the previous one
 * @definition     SDL_DECLSPEC mod_result_t SDLCALL mod_reload_v1(void **userptr) { ... }
 */
typedef mod_result_t (*mod_reload_v1_t)(void **userptr);

//-----------------------------------------------------------------------------
// [SECTION] ECS Core definitions
//-----------------------------------------------------------------------------
//TODO follow the hint to separate in components.* and systems.* to make replacable things
//https://www.flecs.dev/flecs/md_docs_2DesignWithFlecs.html#modules-and-feature-swapping
// ecs-app-core.h

// Phases for pipelines
extern ecs_entity_t RenderingPreImGui, RenderingOnImGui, RenderingPostImGui;

// Components
typedef struct {
	Sint32 running_app_version;
} AppVersion;
SDL_DECLSPEC extern ECS_COMPONENT_DECLARE(AppVersion);

typedef struct {
	// TODO this approach don't cover per-thread arena
	SDL_malloc_func sdl_malloc_func;
	SDL_calloc_func sdl_calloc_func;
	SDL_realloc_func sdl_realloc_func;
	SDL_free_func sdl_free_func;
	ImGuiMemAllocFunc imgui_malloc_func;
	ImGuiMemFreeFunc imgui_free_func;
	void* imgui_allocator_functions_user_data;
	// flecs alloc_funcs can always be retreived with ecs_os_get_api()
} AppMemoryFuncs;
extern SDL_DECLSPEC ECS_COMPONENT_DECLARE(AppMemoryFuncs);

typedef struct {
	SDL_Window *main_window;
	SDL_GPUDevice *gpu_device;
	SDL_AsyncIOQueue *sdl_io_queue;
} AppSDLContext;
extern SDL_DECLSPEC ECS_COMPONENT_DECLARE(AppSDLContext);

typedef struct {
	ImGuiContext* imgui_context;
	ImGuiIO *imgui_io;
} AppImGuiContext;
extern SDL_DECLSPEC ECS_COMPONENT_DECLARE(AppImGuiContext);

typedef struct {
	Uint32 app_iterate_count;
	Uint32 total_skipped;
	Sint32 main_framerate_num;  // AVRational framerate numerator
	Sint32 main_framerate_den;  // AVRational framerate denominator
	Uint64 main_frame_start_ns; // In SDL_GetTicksNS() format, snapped to multiple of main_framerate
	Uint64 main_frameid; // Unique identifier for current frame, garanted monotonic until main_framerate changes
} AppMainTimingContext;
extern SDL_DECLSPEC ECS_COMPONENT_DECLARE(AppMainTimingContext);

// helper called from app-init.c ECS_IMPORT(world, AppCore)
void AppCoreImport(ecs_world_t *world);

// ecs-mods-state.h
extern SDL_DECLSPEC ECS_TAG_DECLARE(ModState);
extern SDL_DECLSPEC ECS_ENTITY_DECLARE(Available);
extern SDL_DECLSPEC ECS_ENTITY_DECLARE(Incompatible);
extern SDL_DECLSPEC ECS_ENTITY_DECLARE(LoadFailed);
extern SDL_DECLSPEC ECS_ENTITY_DECLARE(InitFailed);
extern SDL_DECLSPEC ECS_ENTITY_DECLARE(Running);
extern SDL_DECLSPEC ECS_ENTITY_DECLARE(Terminated);

// helper called from app-init.c ECS_IMPORT(world, ModsLifecycle)
void ModsStateImport(ecs_world_t *world);

// ecs-mods-lifecycle.h
extern SDL_DECLSPEC ECS_TAG_DECLARE(ModFlags);
extern SDL_DECLSPEC ECS_ENTITY_DECLARE(Reloadable);
extern SDL_DECLSPEC ECS_ENTITY_DECLARE(NewerOnDisk);

typedef struct {
	char *name;
	char *so_path;
	SDL_Time modify_time;
} ModOnDisk;
extern SDL_DECLSPEC ECS_COMPONENT_DECLARE(ModOnDisk);

typedef struct {
	void *shared_object;
	void *userptr;
	Sint32 build_dep_version_compiled_against;
	SDL_Time so_file_modify_time_when_loaded_in_ram;
	mod_init_v1_t mod_init_v1;
	mod_reload_v1_t mod_reload_v1;
	mod_fini_v1_t mod_fini_v1;
} ModInRAM;
extern SDL_DECLSPEC ECS_COMPONENT_DECLARE(ModInRAM);

// helper called from app-init.c ECS_IMPORT(world, ModsLifecycle)
void ModsLifecycleImport(ecs_world_t *world);
