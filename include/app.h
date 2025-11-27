#pragma once
#include <SDL3/SDL.h>
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
#define app_trace(args...)    if (!skip_debug) SDL_LogTrace(SDL_LOG_CATEGORY_APPLICATION, args)
#define app_verbose(args...)  if (!skip_debug) SDL_LogVerbose(SDL_LOG_CATEGORY_APPLICATION, args)
#define app_debug(args...)    if (!skip_debug) SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, args)
#define app_info(args...)     SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, args)
#define app_warn(args...)     SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, args)
#define app_error(args...)    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, args)
#define app_critical(args...) SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, args)

typedef struct appstate {
	ecs_world_t *world;
	SDL_Time tick0_wallclock;
	bool skip_debug;
	SDL_Window *window;
	SDL_GPUDevice *gpu_device;
	ImGuiIO *io;
	Sint32 frame_count;
	bool show_demo_window;
	bool show_another_window;
	ImVec4 clear_color;
} appstate_t;
