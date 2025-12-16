#pragma once
#include "mods-api.h"

#define APP_MAX_MODS_COUNT 64
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

typedef struct appmods {
	Sint32 mods_count;

	char *mod_dirname[APP_MAX_MODS_COUNT]; // also a good module display name for logs
	char *mod_sopath[APP_MAX_MODS_COUNT];

	void *shared_object[APP_MAX_MODS_COUNT];
	Sint32 app_version_compiled_against[APP_MAX_MODS_COUNT];
	void *userptr[APP_MAX_MODS_COUNT];

	mod_init_v1_t mod_init_v1[APP_MAX_MODS_COUNT];
	mod_fini_v1_t mod_fini_v1[APP_MAX_MODS_COUNT];

	Sint32 hook_ui_config_v1_count;
	hook_ui_config_v1_t hook_ui_config_v1[APP_MAX_MODS_COUNT];
	Sint32 hook_ui_config_v1_from[APP_MAX_MODS_COUNT];

	Sint32 hook_ui_main_v1_count;
	hook_ui_main_v1_t hook_ui_main_v1[APP_MAX_MODS_COUNT];
	Sint32 hook_ui_main_v1_from[APP_MAX_MODS_COUNT];

	Sint32 hook_ui_menu_v1_count;
	hook_ui_menu_v1_t hook_ui_menu_v1[APP_MAX_MODS_COUNT];
	Sint32 hook_ui_menu_v1_from[APP_MAX_MODS_COUNT];

} appmods_t;

void mod_load_all(appstate_t *appstate);
// Next declaration must be a function compatible with SDL_EnumerateDirectoryCallback
// typedef SDL_EnumerationResult (SDLCALL *SDL_EnumerateDirectoryCallback)(void *userdata, const char *dirname, const char *fname);
SDL_EnumerationResult mod_tryload(void *_appstate, const char *mods_basepath, const char *mod_dirname);
