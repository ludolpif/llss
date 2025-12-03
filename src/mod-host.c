#include "app.h"
#include "app-internal.h"
#include "mod-host.h"

// Forward declarations
void mod_tryload_optionnal_hooks(appmods_t *mods, int i);

SDL_EnumerationResult mod_tryload(void *_appstate, const char *mods_basepath, const char *mod_dirname) {
	appstate_t *appstate = (appstate_t *) _appstate;
	appmods_t *mods = &appstate->internal->mods;
	char *mod_dirpath;
	SDL_PathInfo info;

	// Note: SDL invite us to use "/" as path separator even on Windows, see https://github.com/libsdl-org/SDL/issues/11370
	if (!SDL_asprintf(&mod_dirpath, "%s%s", mods_basepath, mod_dirname)) {
		app_error("%016lu mod_tryload(): mod_tryload(appstate, \"%s\", \"%s\"): SDL_asprintf(...,\"%%s%%s\",...): %s",
				SDL_GetTicksNS(), mods_basepath, mod_dirname, SDL_GetError());
		goto bad4;
	}
	// Silently skip files living at mods_basepath
	if (!SDL_GetPathInfo(mod_dirpath, &info)) {
		app_error("%016lu mod_tryload(): SDL_GetPathInfo(): %s", SDL_GetTicksNS(), SDL_GetError());
		goto bad3;
	}
	if (info.type != SDL_PATHTYPE_DIRECTORY) {
		goto bad3;
	}

	int i = mods->mods_count;
	if ( i == APP_MAX_MODS_COUNT ) {
		app_error("%016lu mod_tryload(): APP_MAX_MODS_COUNT reached", SDL_GetTicksNS());
		goto bad3;
	}

	// Allocate and set mods->mod_dirname[i]
	mods->mod_dirname[i] = SDL_strdup(mod_dirname);

	// Allocate and set mods->mod_sopath[i]
	if (!SDL_asprintf(&mods->mod_sopath[i], "%s%s/%s%s", mods_basepath, mod_dirname, mod_dirname, APP_MOD_FILEEXT)) {
		app_error("%016lu mod_tryload(): mod_tryload(appstate, \"%s\", \"%s\"): SDL_asprintf(...,\"%%s%%s/%%s%%s\",...): %s",
				SDL_GetTicksNS(), mods_basepath, mod_dirname, SDL_GetError());
		goto bad4;
	}

	app_warn("%016lu mod_tryload(): will load %s", SDL_GetTicksNS(), mods->mod_sopath[i]);
	mods->shared_object[i] = SDL_LoadObject(mods->mod_sopath[i]);
	if (!mods->shared_object[i]) {
		app_warn("%016lu mod_tryload(): SDL_LoadObject(): %s", SDL_GetTicksNS(), SDL_GetError());
		goto bad2;
	}

	app_mod_handshake_t app_mod_handshake = (app_mod_handshake_t) SDL_LoadFunction(mods->shared_object[i], "app_mod_handshake");
	if (!app_mod_handshake) {
		app_warn("%016lu mod_tryload(): SDL_LoadFunction(..., app_mod_handshake): %s", SDL_GetTicksNS(), SDL_GetError());
		goto bad;
	}

	mods->app_version_compiled_against[i] = app_mod_handshake(APP_VERSION_INT);
	int minimal_app_version = APP_VERSION_TO_INT(0,1,0); //TODO put in a public header
	if (mods->app_version_compiled_against[i] < APP_VERSION_TO_INT(0,1,0)) {
		app_warn("%016lu app_mod_handshake(): too old app_version_compiled_against: %i"
				"Please update this module. Current minimal_app_version: %i",
				SDL_GetTicksNS(), mods->app_version_compiled_against[i], minimal_app_version);
		goto bad;
	}

	mods->app_mod_init[i] = (app_mod_init_t) SDL_LoadFunction(mods->shared_object[i], "app_mod_init");
	if (!mods->app_mod_init[i]) {
		app_warn("%016lu mod_tryload(): SDL_LoadFunction(..., app_mod_init): %s", SDL_GetTicksNS(), SDL_GetError());
		goto bad;
	}

	mods->app_mod_fini[i] = (app_mod_fini_t) SDL_LoadFunction(mods->shared_object[i], "app_mod_fini");
	if (!mods->app_mod_fini[i]) {
		app_warn("%016lu mod_tryload(): SDL_LoadFunction(..., app_mod_fini): %s", SDL_GetTicksNS(), SDL_GetError());
		goto bad;
	}

	mod_tryload_optionnal_hooks(mods, i);

	SDL_AppResult init_res = mods->app_mod_init[i](appstate, &mods->userptr[i]);
	if (init_res != SDL_APP_CONTINUE) {
		app_warn("%016lu mod_tryload(): app_mod_init(): error %d", SDL_GetTicksNS(), init_res);
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
	return SDL_ENUM_CONTINUE;
}

void mod_tryload_optionnal_hooks(appmods_t *mods, int i) {
	// If the symbol is not exported in the shared object, SDL_LoadFunction will return NULL and it's fine
	int j;
	j = mods->hook_purpose1_count;
	mods->app_mod_hook_purpose1[j] = (app_mod_hook_purpose1_t) SDL_LoadFunction(mods->shared_object[i], "app_mod_hook_purpose1");
	if ( mods->app_mod_hook_purpose1[j] ) {
		mods->app_mod_hook_purpose1_from[j]=i;
		mods->hook_purpose1_count++;
	}

	j = mods->hook_purpose2_count;
	mods->app_mod_hook_purpose2[j] = (app_mod_hook_purpose2_t) SDL_LoadFunction(mods->shared_object[i], "app_mod_hook_purpose2");
	if ( mods->app_mod_hook_purpose2[j] ) {
		mods->app_mod_hook_purpose2_from[j]=i;
		mods->hook_purpose2_count++;
	}
}
