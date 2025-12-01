#define DLL_EXPORT
#include "app.h"
#include "hello.h"

// SDL_DECLSPEC: A macro to tag a symbol as a public API.
// Windows : DLL_EXPORT must be defined before inclusion of SDL.h (here through app.h) to have a non empty SDL_DECLSPEC
// SDLCALL: A macro to set a function's calling conventions.

SDL_DECLSPEC Sint32 SDLCALL app_mod_handshake(Sint32 running_app_version) {
	// Plugin can bail out if it know that it doesn't meant to be used for app version below some release
	if ( running_app_version < APP_VERSION_TO_INT(0,1,12) ) return -1;
	// Plugin returns to app which version of app headers/libs it was compiled for, app decides to continue or not
	return hello(APP_VERSION_INT);
}

SDL_DECLSPEC SDL_AppResult SDLCALL app_mod_init(appstate_t *appstate, void **userptr) {
	if (!appstate || !appstate->imgui_context) {
		return SDL_APP_FAILURE;
	}
	ImGui_SetCurrentContext(appstate->imgui_context);
	if (!appstate->imgui_malloc_func || !appstate->imgui_free_func) {
		return SDL_APP_FAILURE;
	}
	ImGui_SetAllocatorFunctions(appstate->imgui_malloc_func, appstate->imgui_free_func, NULL);

	*userptr = appstate; // This mod don't need an additionnal internal state

	return SDL_APP_CONTINUE;
}

SDL_DECLSPEC SDL_AppResult SDLCALL app_mod_fini(void *userptr) {
	return SDL_APP_CONTINUE;
}

// No app_mod_hook_purpose1 for this mod (doesn't need it)

SDL_DECLSPEC SDL_AppResult SDLCALL app_mod_hook_purpose2(void *userptr) {
	appstate_t *appstate = (appstate_t *) userptr;

	// 3. Show another simple window.
	if (appstate->show_another_window)
	{
		// Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
		ImGui_Begin("Another Window", &appstate->show_another_window, 0);
		ImGui_Text("Hello from another window!");
		if (ImGui_Button("Close Me"))
			appstate->show_another_window = false;
		ImGui_End();
	}

	return SDL_APP_CONTINUE;
}

Sint32 some_private_func(Sint32 a) {
	return a;
}
