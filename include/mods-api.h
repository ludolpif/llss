#pragma once
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

//TODO try calling SDL_GetThreadName(SDL_Thread *thread) on main to get it's real name
typedef enum mod_result {
	MOD_RESULT_INVALID,
	MOD_RESULT_FAILURE,
	MOD_RESULT_SUCCESS,
	MOD_RESULT_CONTINUE
} mod_result_t;

/**
 * @symbol-name    mod_handshake_v1
 * @calling-thread SDL_Main
 * @called-from    program/mod-host.c
 * @when           after SDL_LoadObject(".../this-mod.so")
 * @mandatory      yes
 * @purpose        version compatibility check while a mod is loaded. no data availble yet.
 * @definition     SDL_DECLSPEC Sint32 SDLCALL mod_handshake(Sint32 running_app_version) { ... }
 */
typedef Sint32 (*mod_handshake_v1_t)(Sint32 running_app_version);

/**
 * @symbol-name    mod_init_v1
 * @calling-thread SDL_Main
 * @called-from    program/mod-host.c
 * @when           after mod_handshake_v1 success
 * @mandatory      yes
 * @purpose        mod own state initialization on stack without global variables
 * @param appstate in: main app appstate, this pointer value should be copied in mod main data struct
 * @param userptr  out: userptr that be passed when future hooks will be called, should be SDL_calloc()ed by the mod
 * @definition     SDL_DECLSPEC mod_result_t SDLCALL mod_init_v1(appstate_t *appstate, void **userptr) { ... }
 */
typedef mod_result_t (*mod_init_v1_t)(appstate_t *appstate, void **userptr);

/**
 * @symbol-name    mod_fini_v1
 * @calling-thread SDL_Main
 * @called-from    program/mod-host.c
 * @when           right before SDL_UnloadObject()
 * @mandatory      yes
 * @purpose        mod own state de-initialization (free structs from stack)
 * @definition     SDL_DECLSPEC mod_result_t SDLCALL mod_fini_v1(void *userptr) { ... }
 */
typedef mod_result_t (*mod_fini_v1_t)(void *userptr);

/**
 * @symbol-name    mod_reload_v1
 * @calling-thread SDL_Main
 * @called-from    program/mod-host.c
 * @when           after loading the new one, before unloading the old one
 * @mandatory      yes
 * @purpose        allow mod hot-reloading by given the new one a pointer to the data of the previous one
 * @definition     SDL_DECLSPEC mod_result_t SDLCALL mod_reload_v1(void **userptr) { ... }
 */
typedef mod_result_t (*mod_reload_v1_t)(void **userptr);

// TODO more descriptions
typedef mod_result_t (*hook_ui_config_v1_t)(void *userptr);
typedef mod_result_t (*hook_ui_main_v1_t)(void *userptr);
typedef mod_result_t (*hook_ui_menu_v1_t)(void *userptr);
