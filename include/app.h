#pragma once

#include "dcimgui.h"
#include <SDL3/SDL.h>

#define APP_METADATA_NAME_STRING "llss"
#define APP_METADATA_VERSION_STRING "0.1.0"
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
	SDL_Time tick0_wallclock;
	bool skip_debug;
	SDL_Window *window;
	SDL_GPUDevice *gpu_device;
	ImGuiIO *io;
	bool show_demo_window;
	bool show_another_window;
	ImVec4 clear_color;
} appstate_t;
