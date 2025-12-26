/*
 * This file is part of Foobar.
 *
 * Foobar is free software: you can redistribute it and/or modify it under the terms of the
 * Affero GNU General Public License as published by the Free Software Foundation,
 * either version 3 of the License, or (at your option) any later version.
 *
 * Foobar is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with Foobar.
 * If not, see <https://www.gnu.org/licenses/>. See LICENSE file at root of this git repo.
 *
 * Copyright 2025 Author <author-contact@email-domain.tld>
 */
#define DLL_EXPORT
#define MOD_USES_IMGUI
#include "app.h"
#include "mods-api.h"
#include "hello.h"

/*
 * mod writer quickstart: no main(), no global variables.
 * You have to implement a few mandatory hooks, export them. The app will call them.
 * The full list of available hooks are in "mods-api.h", with extensive details.
 *
 * Windows : macro DLL_EXPORT must be defined before inclusion of SDL.h (here through app.h) to have a non empty SDL_DECLSPEC
 * SDL_DECLSPEC macro purpose: tag something to be exported as a dynamic symbol. Build system hide symbols by default.
 * SDLCALL macro purpose: set a function's calling conventions (unsure if really needed in this project scope, but SDL do it)
 *
 * Parameters
 * void *userptr:
 *  from the app point of view: opaque data defined by the mod, and managed by the mod only.
 *  from the mod: void * to SDL_calloc() private mod data in substitution of global variables
 *   It's important because Windows, Mac, Linux have different behaviors about heap and
 *    namespace separation of dynamically loaded shared objects.
 *   It's allocation/deallocation have to be done in mod_init_v1/mod_fini_v1 and optionnally in mod_reload_v1
 *
 * appstate_t *appstate:
 *  contains all core app informations that must be defined only once in the whole app
 *  contains ecs_world_t *world that allow the mod to create, modify and delete dynamic data and types
 *   and uses data defined by the core app or the other mods.
 */

typedef struct mod_main_data {
	Sint32 running_struct_size;
	appstate_t *appstate;
	/*
	 * Try to put a maximum of data in ECS (appstate->world) instead of here.
	 *  ECS should contain *all* human-readable data (for the user and others mods).
	 * The remaining of this struct should be reserved only for pointers
	 *  and things that your mod dependencies needs to have internally.
	 */
	bool show_another_window;
	//lib_something_internal_data_t lib_something;
	//Sint32 some_fixed_size_typed_data_for_the_mod1;
	//mod_template_something_t *list_of_something;
	//Sint32 list_of_something_len;

	/*
	 * When improving the mod, don't change previous members, add new ones below,
	 *  so you can hot-reload from previous to new version by
	 *  - checking running_struct_size againt current sizeof(mod_main_data_t)
	 *  - SDL_realloc()ing the data
	 *  - initializing the bottom of the struct with zeros
	 *  - put some useful default value to fields that equals to 0
	 */
} mod_main_data_t;

SDL_DECLSPEC Sint32 SDLCALL mod_handshake_v1(Sint32 running_app_version) {
	// Plugin can bail out if it know that it doesn't meant to be used with app version below APP_VERSION_TO_INT(major,minor,patch)
	if ( running_app_version < APP_VERSION_TO_INT(0,1,0) ) return -1;
	// Plugin returns to app which version of app headers/libs it was compiled for, app decides to continue or not
	return APP_VERSION_INT;
}

SDL_DECLSPEC mod_result_t SDLCALL mod_init_v1(appstate_t *appstate, void **userptr) {
#ifdef MOD_USES_IMGUI
	// As ImGui use a notion of global context for it's API calls, use heap to process data
	// and we are in a shared object, we need to ImGui_SetCurrentContext and ImGui_SetAllocatorFunctions again.
	ImGui_SetCurrentContext(appstate->imgui_context);
	ImGui_SetAllocatorFunctions(appstate->imgui_malloc_func, appstate->imgui_free_func, appstate->imgui_allocator_functions_user_data);
#endif
	// TODO check if https://wiki.libsdl.org/SDL3/SDL_GetMemoryFunctions return the custom ones on linux and Windows. I think the mod have nothing to do but, unsure.

	// Allocate our mod private state
	*userptr = SDL_calloc(1, sizeof(mod_main_data_t));
	if (!*userptr) return MOD_RESULT_FAILURE;

	mod_main_data_t *data = (mod_main_data_t *) userptr;
	data->show_another_window = true;

	return MOD_RESULT_CONTINUE;
}

// Mandatory mod_fini hook, called before this mod is fully unloaded from memory
SDL_DECLSPEC mod_result_t SDLCALL mod_fini_v1(void *userptr) {
	SDL_free(userptr);
	return MOD_RESULT_CONTINUE;
}

// Optionnal mod_reload hook
SDL_DECLSPEC mod_result_t SDLCALL mod_reload_v1(void **userptr) {
	// for users: allow to disable a mod to check performance changes or reset things
	// for mod devs: can be used to hot-reload the mod, even with keeping previous data
	//  if mod private data struct definitions matches or if only some members added at end of structs
	//  you may need to check member could be a nice thing to have
	//
	// FIXME try it and make the sequence works (both version in memory ? how to not fuck up ? :D)
	// TODO define what is mandatory to implement on the mod side
	// like check if data structure is the same size or version before going on ?
	return MOD_RESULT_CONTINUE;
}

// Optionnal hooks
SDL_DECLSPEC mod_result_t SDLCALL hook_ui_config_v1(void *userptr) {
	mod_main_data_t *data = (mod_main_data_t *) userptr;
	mod_result_t then = MOD_RESULT_CONTINUE;

	// Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
	ImGui_Begin("A mod Window", &data->show_another_window, 0);
	ImGui_Text("Hello from mod-template!");
	if (ImGui_Button("Close Me"))
		then = MOD_RESULT_SUCCESS;
	ImGui_End();

	// You can also ImGui_Begin("An existing window", NULL), it will add components to it, even if created by app or other plugin !

	//TODO see if we can have ImGui and ECS share the same data copy (&data->show_another_window is not correctly shared for now)
	return then;
}


// This function will not be an exported dynamic symbol because SDL_DECLSPEC is absent
Sint32 some_private_func(Sint32 a) {
	return a;
}
