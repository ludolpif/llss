#include "app.h"
#include "mod-host.h"

SDL_AppResult mod_host_init(appstate_t *appstate) {

	appmods_t *mods = appstate->mods;
	int i=0;

	mods->shared_object[i] = SDL_LoadObject("../mods/llss-mod-template/src/llss-mod-template.so.1");
	if (mods->shared_object[i]) return false;

	app_mod_handshake_t app_mod_handshake = (app_mod_handshake_t) SDL_LoadFunction(mods->shared_object[i], "app_mod_handshake");
	if (!app_mod_handshake) goto bad;

	mods->app_version_compiled_against[i] = app_mod_handshake(APP_VERSION_INT);
	if (mods->app_version_compiled_against[i] < APP_VERSION_TO_INT(0,1,0)) goto bad;

	mods->app_mod_init[i] = (app_mod_init_t) SDL_LoadFunction(mods->shared_object[i], "app_mod_init");
	if (!mods->app_mod_init[i]) goto bad;

	mods->app_mod_fini[i] = (app_mod_fini_t) SDL_LoadFunction(mods->shared_object[i], "app_mod_fini");
	if (!mods->app_mod_fini[i]) goto bad;

	// All further hooks are optional, if the symbol is not exported in the shared object, SDL_LoadFunction will return NULL
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

	mods->mods_count++;

	return SDL_APP_CONTINUE;

bad:
	app_warn("SDL_LoadFunction(): %s", SDL_GetError());
	return SDL_APP_FAILURE;
}

