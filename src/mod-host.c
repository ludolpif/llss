#include "app.h"
#include "mod-host.h"

// Forward declarations
void mod_tryload_optionnal_hooks(appmods_t *mods, int i);

SDL_EnumerationResult mod_tryload(void *_appstate, const char *mods_basepath, const char *mod_dirname) {
	appstate_t *appstate = (appstate_t *) _appstate;
	appmods_t *mods = appstate->mods;

	app_warn("%016lu mod_tryload(appstate, \"%s\", \"%s\"): starting now",
			SDL_GetTicksNS(), mods_basepath, mod_dirname);

	int i = mods->mods_count;
	if ( i == APP_MAX_MODS_COUNT ) {
		app_error("%016lu mod_tryload(): APP_MAX_MODS_COUNT reached", SDL_GetTicksNS());
		goto bad4;
	}

	mods->mod_dirname[i] = SDL_strdup(mod_dirname);

	//TODO make it properly, for all platforms
	if (!SDL_asprintf(&mods->mod_sopath[i], "%s%s/%s.so.1", mods_basepath, mod_dirname, mod_dirname)) {
		app_error("%016lu mod_tryload(): mod_tryload(appstate, \"%s\", \"%s\"): SDL_asprintf: %s",
				SDL_GetTicksNS(), mods_basepath, mod_dirname, SDL_GetError());
		goto bad3;
	}

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

	mods->mods_count++;

	return SDL_ENUM_SUCCESS;

bad:
	SDL_UnloadObject(mods->shared_object[i]);
bad2:
	SDL_free(mods->mod_sopath[i]);
	mods->mod_sopath[i] = NULL;
bad3:
	SDL_free(mods->mod_dirname[i]);
	mods->mod_dirname[i] = NULL;
bad4:
	// We want to skip this module but not prevent tryload of the next one
	return SDL_ENUM_SUCCESS;
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
