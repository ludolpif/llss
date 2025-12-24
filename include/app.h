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
//TODO consider #define IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_USER_CONFIG "imgui_config.h"
#include "dcimgui.h"
#include "flecs.h"

//-----------------------------------------------------------------------------
// [SECTION] General and metadata macro definitions
//-----------------------------------------------------------------------------
#include "metadata.h"
// Following macros can't be in metadata.h as Microsoft res.exe can't cope with it (app.rc includes metadata.h)
#define APP_VERSION_TO_INT(a, b, c) ((a)<<16 | (b)<<8 | (c))
#define	APP_VERSION_MAJOR_FROM_INT(a) ((a) >> 16)
#define	APP_VERSION_MINOR_FROM_INT(a) (((a) & 0x00FF00) >> 8)
#define	APP_VERSION_MICRO_FROM_INT(a) ((a) & 0xFF)

#define APP_VERSION_INT APP_VERSION_TO_INT(APP_VERSION_MAJOR,APP_VERSION_MINOR,APP_VERSION_PATCH)

//-----------------------------------------------------------------------------
// [SECTION] Logging helpers and counters definitions
//-----------------------------------------------------------------------------
/* TODO
typedef enum app_logcategory {
	APP_CATEGORY_CORE = SDL_LOG_CATEGORY_CUSTOM,
	APP_CATEGORY_PLUGIN,
} app_logcategory_t;
*/
//TODO define a general way to count events and let mods to add custom counters, let user display them, some are absolution values, some DERIVATIVE as in RRD

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
// User-defined data and most of human-readable data should go in the ECS, not here.
// Technical data that have to be globally available (or singletons) should be here.
// Global variables are used in very last resort as various OS makes various things with mod dynamic loading.

// We define an opaque type for internal states that are subject to change without breaking mods ABI.
// We want to hide a minimal set of data, but it let room for work-in-progress core things.
struct appinternal;

// App state, used by the app and exposed to mods, should only extended by adding fields at the end
//  Don't use bitfields members as binary layout isn't specification defined (can vary between compilers)
//  Use fixed-size types like Sint32 and not int
typedef struct appstate {
	Sint32 running_app_version;
	struct appinternal *internal;
	SDL_LogPriority logpriority_earlyskip;

	SDL_malloc_func sdl_malloc_func;
	SDL_calloc_func sdl_calloc_func;
	SDL_realloc_func sdl_realloc_func;
	SDL_free_func sdl_free_func;

	ImGuiMemAllocFunc imgui_malloc_func;
	ImGuiMemFreeFunc imgui_free_func;
	void* imgui_allocator_functions_user_data;

	SDL_Time tick0_wallclock;
	Uint32 frameid;

	SDL_Window *window;
	SDL_GPUDevice *gpu_device;
	ImGuiContext* imgui_context;
	ImGuiIO *imgui_io;

	SDL_AsyncIOQueue *sdl_io_queue;
	ecs_world_t *world;

} appstate_t;

//-----------------------------------------------------------------------------
// [SECTION] ECS Core components definitions
//-----------------------------------------------------------------------------
// TODO

