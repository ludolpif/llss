#pragma once

/*
 * Our public API. Kept in one file to help searching... but long, so quick table of content.
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
#define APP_VERSION_TO_INT(a, b, c) ((a)<<16 | (b)<<8 | (c))
#define	APP_VERSION_MAJOR_FROM_INT(a) ((a) >> 16)
#define	APP_VERSION_MINOR_FROM_INT(a) (((a) & 0x00FF00) >> 8)
#define	APP_VERSION_MICRO_FROM_INT(a) ((a) & 0xFF)

#define APP_VERSION_INT APP_VERSION_TO_INT(0,2,1)
#define APP_VERSION_STR "0.2.1"

#define APP_METADATA_NAME_STRING "llss"
#define APP_METADATA_VERSION_STRING APP_VERSION_STR
#define APP_METADATA_IDENTIFIER_STRING "todo"
#define APP_METADATA_CREATOR_STRING "ludolpif"
#define APP_METADATA_COPYRIGHT_STRING "Copyright 2025-now ludolpif <ludolpif@gmail.com>"
#define APP_METADATA_URL_STRING "https://ludolpif.fr/llss"
#define APP_METADATA_TYPE_STRING "application"

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
#define app_trace(...)    if (logpriority_earlyskip > SDL_LOG_PRIORITY_TRACE) SDL_LogTrace(SDL_LOG_CATEGORY_APPLICATION, __VA_ARGS__)
#define app_verbose(...)  if (logpriority_earlyskip > SDL_LOG_PRIORITY_VERBOSE) SDL_LogVerbose(SDL_LOG_CATEGORY_APPLICATION, __VA_ARGS__)
#define app_debug(...)    if (logpriority_earlyskip > SDL_LOG_PRIORITY_DEBUG) SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, __VA_ARGS__)
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
	Sint32 frameid;

	ecs_world_t *world;
	SDL_Window *window;
	SDL_GPUDevice *gpu_device;
	ImGuiContext* imgui_context;
	ImGuiIO *imgui_io;

	//TODO this is temporary, move in appinternal
	bool show_demo_window;
	bool show_another_window;
	ImVec4 clear_color;
} appstate_t;

//-----------------------------------------------------------------------------
// [SECTION] ECS Core components definitions
//-----------------------------------------------------------------------------
// TODO

