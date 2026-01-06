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
#include "app.h"
#include "hello.h"

#define MOD_USES_IMGUI
/*
 * mod writer quickstart: no main(), no global variables.
 * You have to implement a few mandatory hooks, export them. The app will call them.
 * You get access to the main app ecs_world_t *world and using to flecs API,
 *  you can register Entities, Components, Systems, Tasks, Observers
 *  to extend the data set known globally and extend the behavior of the app.
 *
 * MOD_API macro purpose: tag something to be exported or imported as a dynamic symbol.
 *  Note that the build system is configured to hide symbols by default.
 * SDLCALL macro purpose: set a function's calling conventions to C calling convention
 *  (unsure if really needed in this project scope, but SDL do it, important if you mix some C++)
 *
 * Parameters
 * void *userptr:
 *  from the app point of view: opaque data defined by the mod, and managed by the mod only.
 *  from the mod: points to a struct for private mod data in substitution of global variables.
 *   It's important because Windows, Mac, Linux have different behaviors about heap and
 *    namespace separation of dynamically loaded shared objects. Allow also some mod reload features.
 *   It's allocation/deallocation have to be done in mod_init_v1/mod_fini_v1 and optionnally in
 *   mod_reload_v1 using SDL_calloc()
 *
 * ecs_world_t *world:
 *  contains all core app and others mods informations and behaviors
 *  FLECS API allow the mod to create, modify and delete dynamic data and types
 *   and uses data defined by the core app or the other mods.
 */

typedef struct {
    size_t running_struct_size;
    ecs_world_t *world;
    /*
     * Try to put a maximum of data in ECS (*world) instead of below.
     *  ECS should contain *all* human-readable data (for the user and others mods).
     * The remaining of this struct should be reserved only for pointers
     *  and things that your mod dependencies needs to have internally.
     */
    //lib_something_internal_data_t lib_something;
    //int32_t some_fixed_size_typed_data_for_the_mod1;
    //mod_template_something_t *list_of_something;
    //int32_t list_of_something_len;

    /*
     * When improving the mod, don't change previous members, add new ones below,
     *  so you can hot-reload from previous to new version by
     *  - checking running_struct_size againt current sizeof(mod_main_data_t)
     *  - SDL_realloc()ing the data
     *  - initializing the bottom of the struct with zeros
     *  - put some useful default value to fields that equals to 0
     *  - decide to reset some values
     */
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
    app_debug("%016"PRIu64" mod-template mod_init_v1(%p, %"PRIu32", %p)",
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

    // ECS_IMPORT is valid because mod_init_v1() called outside ecs_progress() by sdl-app-iterate.c
    ECS_IMPORT(world, ModTemplateHello); // Creates entity with Module component then many others
                                         // It's query/lookup identifier is "mod.template.hello"

    return MOD_RESULT_SUCCESS;
}

MOD_API mod_result_t SDLCALL mod_fini_v1(uint32_t flags, void *userptr) {
    app_debug("%016"PRIu64" mod-template mod_fini_v1(%"PRIu32", %p)",
            SDL_GetTicksNS(), flags, userptr);

    mod_main_data_t *data = (mod_main_data_t *) userptr;
    ecs_world_t *world = data->world;

    // Remove all ECS items registered by this mod (if name/prefix convention was respected)
    ecs_entity_t e_hello = ecs_lookup(world, "mod.template");
    if (e_hello) {
        ecs_delete(world, e_hello);
        app_debug("%016"PRIu64" mod_fini_v1(): removed ECS items", SDL_GetTicksNS());
    }

    // here may need to add calls to shutdown to your mod's dependencies

    if (!(flags & MOD_FLAGS_RELOADING)) {
        SDL_free(userptr); // last operation to do
    }
    return MOD_RESULT_SUCCESS;
}

// This function will not be an exported dynamic symbol because MOD_API is absent
int32_t some_private_func(int32_t a) {
    return a;
}
