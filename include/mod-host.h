#pragma once

typedef int  (*app_mod_handshake_t)(int);
typedef bool (*app_mod_init_t)(void);
typedef void (*app_mod_fini_t)(void);
typedef void (*app_mod_hook_purpose1_t)(void);
typedef void (*app_mod_hook_purpose2_t)(void);

#define APP_MAX_MODS_COUNT 64
typedef struct appmods {
	int mods_count;
	char *mod_dirname[APP_MAX_MODS_COUNT];
	char *mod_sopath[APP_MAX_MODS_COUNT];
	void *shared_object[APP_MAX_MODS_COUNT];
	int app_version_compiled_against[APP_MAX_MODS_COUNT];
	app_mod_init_t app_mod_init[APP_MAX_MODS_COUNT];
	app_mod_fini_t app_mod_fini[APP_MAX_MODS_COUNT];

	int hook_purpose1_count;
	app_mod_hook_purpose1_t app_mod_hook_purpose1[APP_MAX_MODS_COUNT];
	int app_mod_hook_purpose1_from[APP_MAX_MODS_COUNT];

	int hook_purpose2_count;
	app_mod_hook_purpose2_t app_mod_hook_purpose2[APP_MAX_MODS_COUNT];
	int app_mod_hook_purpose2_from[APP_MAX_MODS_COUNT];
} appmods_t;

// Next declaration must be a function compatible with SDL_EnumerateDirectoryCallback
// typedef SDL_EnumerationResult (SDLCALL *SDL_EnumerateDirectoryCallback)(void *userdata, const char *dirname, const char *fname);
SDL_EnumerationResult mod_tryload(void *_appstate, const char *mods_basepath, const char *mod_dirname);
