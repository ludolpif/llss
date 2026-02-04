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
 */
// Quick search table:
// [SECTION] Conventions
// [SECTION] General and metadata macro definitions
// [SECTION] Boring stuff needed for symbol visibility
// [SECTION] Logging helpers and counters definitions
// [SECTION] Libraries directly usable from mods
// [SECTION] Structures for global-like plain old data
// [SECTION] Mods API definition
// [SECTION] ECS Core definitions
// [SECTION] Utility functions

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
// [SECTION] General and metadata macro definitions
//-----------------------------------------------------------------------------
#include "version-app.h"
#include "version-build-dep.h"
// Following macros can't be in *-version.h as Microsoft res.exe can't cope with it (app.rc includes them)
#define VERSION_TO_INT(a, b, c) (a*10000+b*100+c)
#define    VERSION_MAJOR_FROM_INT(a) (a/10000)
#define    VERSION_MINOR_FROM_INT(a) ((a%10000)/100)
#define    VERSION_MICRO_FROM_INT(a) (a%100)

#define APP_VERSION_INT VERSION_TO_INT(APP_VERSION_MAJOR,APP_VERSION_MINOR,APP_VERSION_PATCH)
#define BUILD_DEP_VERSION_INT VERSION_TO_INT(BUILD_DEP_VERSION_MAJOR,BUILD_DEP_VERSION_MINOR,BUILD_DEP_VERSION_PATCH)

//-----------------------------------------------------------------------------
// [SECTION] Boring stuff needed for symbol visibility
//-----------------------------------------------------------------------------
#include <SDL3/SDL_platform_defines.h>
#ifdef app_EXPORTS
# if defined(SDL_PLATFORM_WINDOWS)
#  define APP_API __declspec(dllexport)
# else
#  if defined(__GNUC__) && __GNUC__ >= 4
#   define APP_API __attribute__ ((visibility("default")))
#  else
#   define APP_API
#  endif
# endif
#else
# if defined(SDL_PLATFORM_WINDOWS)
#  define APP_API __declspec(dllimport)
# else
#  define APP_API
# endif
#endif

#if defined(SDL_PLATFORM_WINDOWS)
# define MOD_API __declspec(dllexport)
#else
# if defined(__GNUC__) && __GNUC__ >= 4
#  define MOD_API __attribute__ ((visibility("default")))
# else
#  define MOD_API
# endif
#endif

//-----------------------------------------------------------------------------
// [SECTION] Logging helpers and counters definitions
//-----------------------------------------------------------------------------
#include <SDL3/SDL_stdinc.h>
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

void dmon_error_to_sdl_log_adapter(const char *msg);
void dmon_debug_to_sdl_log_adapter(const char *msg);
void flecs_to_sdl_log_adapter(int32_t level, const char *file, int32_t line, const char *msg);

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

// https://github.com/septag/dmon
#define DMON_MALLOC SDL_malloc
#define DMON_FREE SDL_free
#define DMON_REALLOC SDL_realloc
#define DMON_ASSERT SDL_assert
#define _DMON_LOG_ERRORF(str, ...) do { char msg[512]; SDL_snprintf(msg, sizeof(msg), str, __VA_ARGS__); DMON_LOG_ERROR(msg); } while(SDL_NULL_WHILE_LOOP_CONDITION);
#define _DMON_LOG_DEBUGF(str, ...) do { char msg[512]; SDL_snprintf(msg, sizeof(msg), str, __VA_ARGS__); DMON_LOG_DEBUG(msg); } while(SDL_NULL_WHILE_LOOP_CONDITION);
#define DMON_LOG_ERROR dmon_error_to_sdl_log_adapter
#define DMON_LOG_DEBUG dmon_debug_to_sdl_log_adapter
#define DMON_API_DECL APP_API
#define DMON_API_IMPL APP_API
#define DMON_MAX_PATH 260
#define DMON_MAX_WATCHES 64
#define DMON_SLEEP_INTERVAL 100
// TODO uninvestigated deadlock if DMON_SLEEP_INTERVAL 1000
#include "dmon.h"

//-----------------------------------------------------------------------------
// [SECTION] Structures for global-like plain old data
//-----------------------------------------------------------------------------
extern APP_API SDL_LogPriority logpriority_earlyskip;
extern APP_API Uint32 APP_USER_EVENT_FILESYSTEM;

//-----------------------------------------------------------------------------
// [SECTION] Mods API definition
//-----------------------------------------------------------------------------
typedef enum mod_result {
    MOD_RESULT_INVALID,
    MOD_RESULT_FAILURE,
    MOD_RESULT_SUCCESS,
} mod_result_t;
#define MOD_FLAGS_NONE 0
#define MOD_FLAGS_RELOADING 1<<0
/**
 * @symbol-name    mod_handshake_v1
 * @calling-thread SDL_Main
 * @called-from    program/mod-host.c
 * @when           after SDL_LoadObject(".../this-mod.so")
 * @mandatory      yes
 * @purpose        version compatibility check while a mod is loaded. no data availble yet.
 * @definition     MOD_API int32_t SDLCALL mod_handshake(int32_t running_app_version) { ... }
 */
typedef int32_t (*mod_handshake_v1_t)(int32_t running_app_version);

/**
 * @symbol-name    mod_init_v1
 * @calling-thread SDL_Main
 * @called-from    program/mod-host.c
 * @when           after mod_handshake_v1 success
 * @mandatory      yes
 * @purpose        mod own state initialization
 * @param world    in: main app ECS world, this pointer value should be copied in mod main data struct
 * @param userptr  out: userptr that be passed when future hooks will be called, should be SDL_calloc()ed by the mod
 * @definition     MOD_API mod_result_t SDLCALL mod_init_v1(ecs_world_t *world, uint32_t flags, void **userptr) { ... }
 */
typedef mod_result_t (*mod_init_v1_t)(ecs_world_t *world, uint32_t flags, void **userptr);

/**
 * @symbol-name    mod_fini_v1
 * @calling-thread SDL_Main
 * @called-from    program/mod-host.c
 * @when           right before SDL_UnloadObject()
 * @mandatory      yes
 * @purpose        mod own state de-initialization (free structs from stack)
 * @definition     MOD_API mod_result_t SDLCALL mod_fini_v1(void *userptr) { ... }
 */
typedef mod_result_t (*mod_fini_v1_t)(uint32_t flags, void *userptr);

//-----------------------------------------------------------------------------
// [SECTION] ECS Core definitions
//-----------------------------------------------------------------------------
//https://www.flecs.dev/flecs/md_docs_2DesignWithFlecs.html#modules-and-feature-swapping
#include "app-components-core.h"
#include "app-components-composition.h"
#include "app-components-mods.h"

//-----------------------------------------------------------------------------
// [SECTION] Utility functions
//-----------------------------------------------------------------------------

APP_API char * strdup_or_null(const char *s);
// Placeholder for future l10n support
APP_API const char * _(const char *string_to_localize);

APP_API uint64_t convert_ns_to_frameid(uint64_t ns, int32_t framerate_num, int32_t framerate_den);
APP_API uint64_t convert_frameid_to_ns(uint64_t frameid, int32_t framerate_num, int32_t framerate_den);

// For static initializers, we cannot use the function, so here a macro
#define CONVERT_FRAMEID_TO_NS(frameid, framerate_num, framerate_den) \
    ( ( ( (uint64_t)frameid) * 1000000000 * framerate_den ) / framerate_num )

void push_filesystem_event_to_sdl_queue(dmon_watch_id watch_id, dmon_action action,
        const char* rootdir, const char* filepath, const char* oldfilepath, void* user);
