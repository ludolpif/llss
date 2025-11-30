#pragma once
#include <SDL3/SDL.h>
#define IMGUI_USER_CONFIG "imgui_config.h"
#include "dcimgui.h"
#include "flecs.h"

#define APP_VERSION_TO_DOT(a, b, c) a ##.## b ##.## c
#define APP_VERSION_TO_INT(a, b, c) ((a)<<16 | (b)<<8 | (c))
#define	APP_VERSION_MAJOR_FROM_INT(a) ((a) >> 16)
#define	APP_VERSION_MINOR_FROM_INT(a) (((a) & 0x00FF00) >> 8)
#define	APP_VERSION_MICRO_FROM_INT(a) ((a) & 0xFF)

#define APP_VERSION_INT APP_VERSION_TO_INT(0,2,1)
#define APP_VERSION_STR "0.2.1" //TODO broken APP_VERSION_TO_DOT(0,2,1)

#define APP_METADATA_NAME_STRING "llss"
#define APP_METADATA_VERSION_STRING APP_VERSION_STR
#define APP_METADATA_IDENTIFIER_STRING "todo"
#define APP_METADATA_CREATOR_STRING "ludolpif"
#define APP_METADATA_COPYRIGHT_STRING "Copyright 2025-now ludolpif <ludolpif@gmail.com>"
#define APP_METADATA_URL_STRING "https://ludolpif.fr/llss"
#define APP_METADATA_TYPE_STRING "application"

/* TODO
typedef enum APP_LogCategory {
	APP_CATEGORY_CORE = SDL_LOG_CATEGORY_CUSTOM,
	APP_CATEGORY_PLUGIN,
} APP_LogCategory;
*/
#define app_trace(...)    if (!skip_debug) SDL_LogTrace(SDL_LOG_CATEGORY_APPLICATION, __VA_ARGS__)
#define app_verbose(...)  if (!skip_debug) SDL_LogVerbose(SDL_LOG_CATEGORY_APPLICATION, __VA_ARGS__)
#define app_debug(...)    if (!skip_debug) SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, __VA_ARGS__)
#define app_info(...)     SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, __VA_ARGS__)
#define app_warn(...)     SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, __VA_ARGS__)
#define app_error(...)    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, __VA_ARGS__)
#define app_critical(...) SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, __VA_ARGS__)

// Opaque type for app internal states that are subject to change without breaking mods ABI
struct appinternal;

// App state, used by the app and exposed to mods, should only extended by adding fields at the end
// Don't use bitfields members as binary layout isn't specification defined (can vary between compilers)
typedef struct appstate {
	Sint32 running_app_version;
	struct appinternal *internal;
	bool skip_debug;

	//TODO Check if this necessary (may SDL do it by himself after dlopen ?
	SDL_malloc_func sdl_malloc_func;
	SDL_calloc_func sdl_calloc_func;
	SDL_realloc_func sdl_realloc_func;
	SDL_free_func sdl_free_func;

	ImGuiMemAllocFunc imgui_malloc_func;
	ImGuiMemFreeFunc imgui_free_func;

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
