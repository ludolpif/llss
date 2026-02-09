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
#include "app.h"
#include "compositing.h"

#define MOD_NAME "mod-scratchpad"
#define MOD_ECS_PREFIX "mod.scratchpad"
#define MOD_USES_IMGUI
typedef struct {
    size_t running_struct_size;
    ecs_world_t *world;
} mod_main_data_t;

MOD_API int32_t SDLCALL mod_handshake_v1(int32_t running_app_version) {
    // Plugin can bail out if it know that it doesn't meant to be used
    //  with app version below VERSION_TO_INT(major,minor,patch)
    if ( running_app_version < VERSION_TO_INT(0,5,0) ) return -1;
    // Plugin returns to app which version of app headers/libs
    //  it was compiled for, app decides to continue or not
    return BUILD_DEP_VERSION_INT;
}

MOD_API mod_result_t SDLCALL mod_init_v1(ecs_world_t *world, uint32_t flags, void **userptr) {
    app_debug("%016"PRIu64" "MOD_NAME" mod_init_v1(%p, %"PRIu32", %p)",
            SDL_GetTicksNS(), world, flags, *userptr);
#ifdef MOD_USES_IMGUI
    // As ImGui use a global context for it's API calls, use heap to process data and we are in
    // a shared object, we need to ImGui_SetCurrentContext and ImGui_SetAllocatorFunctions again.
    const AppImGuiContext *imgui = ecs_singleton_get(world, AppImGuiContext);
    ImGui_SetCurrentContext(imgui->imgui_context);
    const AppMemoryFuncs *mem = ecs_singleton_get(world, AppMemoryFuncs);
    ImGui_SetAllocatorFunctions(mem->imgui_malloc_func, mem->imgui_free_func, mem->imgui_allocator_functions_user_data);
    // Both SDL3 and flecs don't need this sort of care.
#endif

    mod_main_data_t *data = NULL;
    if (!(flags & MOD_FLAGS_RELOADING)) {
        // Allocate our mod private state
        *userptr = SDL_calloc(1, sizeof(mod_main_data_t));
        if (!*userptr) return MOD_RESULT_FAILURE;
        // Setup initial state
        data = (mod_main_data_t *) *userptr;
        data->running_struct_size = sizeof(mod_main_data_t);
        data->world = world;
    } else {
        // Use the previous state, using realloc() if fields have been added
        data = (mod_main_data_t *) *userptr;
        if ( data->running_struct_size < sizeof(mod_main_data_t) ) {
            *userptr = SDL_realloc(*userptr, sizeof(mod_main_data_t));
            data = (mod_main_data_t *) *userptr;
            size_t extralen = sizeof(mod_main_data_t) - data->running_struct_size;
            SDL_memset(data+data->running_struct_size, 0, extralen);
            data->running_struct_size = sizeof(mod_main_data_t);
        }
    }

    ECS_IMPORT(world, ModScratchpadCompositing);
    // It's query/lookup identifier is "mod.scratchpad.compositing"

    return MOD_RESULT_SUCCESS;
}

MOD_API mod_result_t SDLCALL mod_fini_v1(uint32_t flags, void *userptr) {
    app_debug("%016"PRIu64" "MOD_NAME" mod_fini_v1(%"PRIu32", %p)",
            SDL_GetTicksNS(), flags, userptr);
    if (!userptr) return MOD_RESULT_FAILURE;

    mod_main_data_t *data = (mod_main_data_t *) userptr;
    ecs_world_t *world = data->world;

    // Remove all ECS items registered by this mod (if name/prefix convention was respected)
    ecs_entity_t e_root = ecs_lookup(world, MOD_ECS_PREFIX);
    if (e_root) {
        ecs_delete(world, e_root);
        app_debug("%016"PRIu64" mod_fini_v1(): removed ECS items", SDL_GetTicksNS());
    }

    // here may need to add calls to shutdown to your mod's dependencies

    if (!(flags & MOD_FLAGS_RELOADING)) {
        SDL_free(userptr); // last operation to do
    }
    return MOD_RESULT_SUCCESS;
}
