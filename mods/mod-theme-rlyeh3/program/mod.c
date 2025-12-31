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
 */
#define MOD_USES_IMGUI
#include "app.h"
#include "ui-theming.h"

Sint32 SDLCALL mod_handshake_v1(Sint32 running_app_version) {
	// Plugin can bail out if it know that it doesn't meant to be used with app version below VERSION_TO_INT(major,minor,patch)
	if ( running_app_version < VERSION_TO_INT(0,2,5) ) return -1;
	// Plugin returns to app which version of app headers/libs it was compiled for, app decides to continue or not
	return BUILD_DEP_VERSION_INT;
}

mod_result_t SDLCALL mod_init_v1(ecs_world_t *world, void **userptr) {
#ifdef MOD_USES_IMGUI
	// As ImGui use a notion of global context for it's API calls, use heap to process data
	// and we are in a shared object, we need to ImGui_SetCurrentContext and ImGui_SetAllocatorFunctions again.
	const AppImGuiContext *imgui = ecs_singleton_get(world, AppImGuiContext);
	ImGui_SetCurrentContext(imgui->imgui_context);
	const AppMemoryFuncs *mem = ecs_singleton_get(world, AppMemoryFuncs);
	ImGui_SetAllocatorFunctions(mem->imgui_malloc_func, mem->imgui_free_func, mem->imgui_allocator_functions_user_data);
#endif
	// This mod don't need an additionnal internal state but you can SDL_calloc() one here
	*userptr = world;

	// Set an ImGui theme from this plugin, to demonstrate modding possibilities
	// V3 theme v1.1
	// - rlyeh, public domain
	igThemeV3('P','Y','C', 0, 0, 1, 1);


	return MOD_RESULT_CONTINUE;
}

// Mandatory mod_fini hook, called before this mod is fully unloaded from memory
mod_result_t SDLCALL mod_fini_v1(void *userptr) {
	return MOD_RESULT_CONTINUE;
}

// Optionnal mod_reload hook
mod_result_t SDLCALL mod_reload_v1(void **userptr) {
	// Set an ImGui theme from this plugin, to demonstrate modding possibilities
	// V3 theme v1.1
	// - rlyeh, public domain
	igThemeV3('P','Y','C', 0, 0, 1, 1);
	return MOD_RESULT_CONTINUE;
}
