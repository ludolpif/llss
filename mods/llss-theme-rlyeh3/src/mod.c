#define DLL_EXPORT
#define MOD_USES_IMGUI
#include "app.h"
#include "ui-theming.h"

// SDL_DECLSPEC: A macro to tag a symbol as a public API.
// Windows : DLL_EXPORT must be defined before inclusion of SDL.h (here through app.h) to have a non empty SDL_DECLSPEC
// SDLCALL: A macro to set a function's calling conventions.

// Mandatory app_mod_handshake, app and mod can detect a known incompatibility
SDL_DECLSPEC Sint32 SDLCALL app_mod_handshake(Sint32 running_app_version) {
	// Plugin can bail out if it know that it doesn't meant to be used for app version below some release
	if ( running_app_version < APP_VERSION_TO_INT(0,1,12) ) return -1;
	// Plugin returns to app which version of app headers/libs it was compiled for, app decides to continue or not
	return APP_VERSION_INT;
}

// Mandatory app_mod_init hook, called right after app_mod_handshake (if succeded) to initialize it's components and state
SDL_DECLSPEC SDL_AppResult SDLCALL app_mod_init(appstate_t *appstate, void **userptr) {
#ifdef MOD_USES_IMGUI
	if (!appstate || !appstate->imgui_context) {
		return SDL_APP_FAILURE;
	}
	ImGui_SetCurrentContext(appstate->imgui_context);
	if (!appstate->imgui_malloc_func || !appstate->imgui_free_func) {
		return SDL_APP_FAILURE;
	}
	ImGui_SetAllocatorFunctions(appstate->imgui_malloc_func, appstate->imgui_free_func, NULL);
#endif
	// This mod don't need an additionnal internal state but you can SDL_calloc() one here
	*userptr = appstate;

	// Set an ImGui theme from this plugin, to demonstrate modding possibilities
	// V3 theme v1.1
	// - rlyeh, public domain
	igThemeV3('P','Y','C', 0, 0, 1, 3);


	return SDL_APP_CONTINUE;
}

// Mandatory app_mod_fini hook, called before this mod is fully unloaded from memory
SDL_DECLSPEC SDL_AppResult SDLCALL app_mod_fini(void *userptr) {
	// SDL_free(userptr); // Only if you SDL_calloc() one in app_mod_init()
	return SDL_APP_CONTINUE;
}

// Optionnal app_mod_reload hook
SDL_DECLSPEC SDL_AppResult SDLCALL app_mod_reload(void *userptr) {
	// TODO define what is mandatory to implement on the mod side
	// like check if data structure is the same size or version before going on ?
	return SDL_APP_CONTINUE;
}

// No app_mod_hook_* for this mod (doesn't need it)
