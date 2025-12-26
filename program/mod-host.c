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
#include "app-internal.h"
#include "mod-host.h"

// Forward declarations
void mod_tryload_optionnal_hooks(appmods_t *mods, int i, SDL_LogPriority logpriority_earlyskip);

// Implementations
void mod_load_all(appstate_t *appstate) {
	char *mods_basepath;

	if (!SDL_asprintf(&mods_basepath, APP_MOD_PATH_FROM_BASEPATH, SDL_GetBasePath())) {
		app_error("%016"PRIu64" mod_load_all(): SDL_asprintf(&mods_basepath, ...) failed", SDL_GetTicksNS());
		return;
	}

	if (!SDL_EnumerateDirectory(mods_basepath, mod_tryload, appstate)) {
		app_error("%016"PRIu64" mod_load_all(): SDL_EnumerateDirectory(%s) failed", SDL_GetTicksNS(), mods_basepath);
	}
	// The module loading effectively happens in mods_tryload(void *_appstate) function/callback. See below.
}

SDL_EnumerationResult mod_tryload(void *_appstate, const char *mods_basepath, const char *mod_dirname) {

/* TODO consider using the ECS instead of appstate->internal->mods;

	appstate_t *appstate = (appstate_t *) _appstate;

	appmods_t *mods = &appstate->internal->mods;
	char *mod_dirpath;
	SDL_PathInfo info;

	// Note: SDL invite us to use "/" as path separator even on Windows, see https://github.com/libsdl-org/SDL/issues/11370
	if (!SDL_asprintf(&mod_dirpath, "%s%s", mods_basepath, mod_dirname)) {
		app_error("%016"PRIu64" mod_tryload(appstate, \"%s\", \"%s\"): SDL_asprintf(...,\"%%s%%s\",...): %s",
				SDL_GetTicksNS(), mods_basepath, mod_dirname, SDL_GetError());
		goto bad4;
	}
	// Silently skip files living at mods_basepath
	if (!SDL_GetPathInfo(mod_dirpath, &info)) {
		app_error("%016"PRIu64" mod_tryload(): SDL_GetPathInfo(): %s", SDL_GetTicksNS(), SDL_GetError());
		goto bad3;
	}
	if (info.type != SDL_PATHTYPE_DIRECTORY) {
		goto bad3;
	}

	int i = mods->mods_count;
	if (i == APP_MAX_MODS_COUNT) {
		app_error("%016"PRIu64" mod_tryload(): APP_MAX_MODS_COUNT reached", SDL_GetTicksNS());
		goto bad3;
	}

	// Allocate and set mods->mod_dirname[i]
	mods->mod_dirname[i] = SDL_strdup(mod_dirname);

	// Allocate and set mods->mod_sopath[i]
	if (!SDL_asprintf(&mods->mod_sopath[i], "%s%s/"APP_MOD_SUBDIR"%s"APP_MOD_FILEEXT,
			       	mods_basepath, mod_dirname, mod_dirname)) {
		app_error("%016"PRIu64" mod_tryload(): mod_tryload(appstate, \"%s\", \"%s\"): SDL_asprintf(...,\"%%s%%s/%%s%%s\",...): %s",
				SDL_GetTicksNS(), mods_basepath, mod_dirname, SDL_GetError());
		goto bad4;
	}

	app_warn("%016"PRIu64" mod_tryload(): %s", SDL_GetTicksNS(), mods->mod_sopath[i]);
	mods->shared_object[i] = SDL_LoadObject(mods->mod_sopath[i]);
	if (!mods->shared_object[i]) {
		app_warn("%016"PRIu64" mod_tryload(): SDL_LoadObject(): %s", SDL_GetTicksNS(), SDL_GetError());
		goto bad2;
	}

	mod_handshake_v1_t mod_handshake_v1 = (mod_handshake_v1_t) SDL_LoadFunction(mods->shared_object[i], "mod_handshake_v1");
	if (!mod_handshake_v1) {
		app_warn("%016"PRIu64" mod_tryload(): SDL_LoadFunction(..., mod_handshake_v1): %s", SDL_GetTicksNS(), SDL_GetError());
		goto bad;
	}

	mods->app_version_compiled_against[i] = mod_handshake_v1(APP_VERSION_INT);
	int minimal_app_version = APP_VERSION_TO_INT(0,1,0); //TODO put in a public header
	if (mods->app_version_compiled_against[i] < APP_VERSION_TO_INT(0,1,0)) {
		app_warn("%016"PRIu64" mod_handshake_v1(): too old app_version_compiled_against: %i"
				"Please update this module. Current minimal_app_version: %i",
				SDL_GetTicksNS(), mods->app_version_compiled_against[i], minimal_app_version);
		goto bad;
	}

	mods->mod_init_v1[i] = (mod_init_v1_t) SDL_LoadFunction(mods->shared_object[i], "mod_init_v1");
	if (!mods->mod_init_v1[i]) {
		app_warn("%016"PRIu64" mod_tryload(): SDL_LoadFunction(..., mod_init_v1): %s", SDL_GetTicksNS(), SDL_GetError());
		goto bad;
	}

	mods->mod_fini_v1[i] = (mod_fini_v1_t) SDL_LoadFunction(mods->shared_object[i], "mod_fini_v1");
	if (!mods->mod_fini_v1[i]) {
		app_warn("%016"PRIu64" mod_tryload(): SDL_LoadFunction(..., mod_fini_v1): %s", SDL_GetTicksNS(), SDL_GetError());
		goto bad;
	}

	mod_tryload_optionnal_hooks(mods, i, appstate->logpriority_earlyskip);

	mod_result_t init_res = mods->mod_init_v1[i](appstate, &mods->userptr[i]);
	if (init_res != MOD_RESULT_CONTINUE) {
		app_warn("%016"PRIu64" mod_tryload(): mod_init_v1(): error %d", SDL_GetTicksNS(), init_res);
		goto bad;
	}

	mods->mods_count++;
	return SDL_ENUM_CONTINUE;

bad:
	SDL_UnloadObject(mods->shared_object[i]);
bad2:
	SDL_free(mods->mod_sopath[i]);
	mods->mod_sopath[i] = NULL;
	SDL_free(mods->mod_dirname[i]);
	mods->mod_dirname[i] = NULL;
bad3:
	SDL_free(mod_dirpath);
bad4:
	// We want to skip this module but not prevent tryload of the next one
*/
	return SDL_ENUM_CONTINUE;
}

void mod_tryload_optionnal_hooks(appmods_t *mods, int i, SDL_LogPriority logpriority_earlyskip) {
	// If the symbol is not exported in the shared object, SDL_LoadFunction will return NULL and it's fine
	int j;
	j = mods->hook_ui_config_v1_count;
	mods->hook_ui_config_v1[j] = (hook_ui_config_v1_t) SDL_LoadFunction(mods->shared_object[i], "hook_ui_config_v1");
	if ( mods->hook_ui_config_v1[j] ) {
		mods->hook_ui_config_v1_from[j]=i;
		mods->hook_ui_config_v1_count++;
	} else {
		app_debug("%016"PRIu64" mod_tryload(): SDL_LoadFunction(..., hook_ui_config_v1): %s", SDL_GetTicksNS(), SDL_GetError());
	}

	j = mods->hook_ui_main_v1_count;
	mods->hook_ui_main_v1[j] = (hook_ui_main_v1_t) SDL_LoadFunction(mods->shared_object[i], "hook_ui_main_v1");
	if ( mods->hook_ui_main_v1[j] ) {
		mods->hook_ui_main_v1_from[j]=i;
		mods->hook_ui_main_v1_count++;
	} else {
		app_debug("%016"PRIu64" mod_tryload(): SDL_LoadFunction(..., hook_ui_main_v1): %s", SDL_GetTicksNS(), SDL_GetError());
	}

	j = mods->hook_ui_menu_v1_count;
	mods->hook_ui_menu_v1[j] = (hook_ui_menu_v1_t) SDL_LoadFunction(mods->shared_object[i], "hook_ui_menu_v1");
	if ( mods->hook_ui_menu_v1[j] ) {
		mods->hook_ui_menu_v1_from[j]=i;
		mods->hook_ui_menu_v1_count++;
	} else {
		app_debug("%016"PRIu64" mod_tryload(): SDL_LoadFunction(..., hook_ui_menu_v1): %s", SDL_GetTicksNS(), SDL_GetError());
	}
}
