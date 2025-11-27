#include "app.h"
#include "hello.h"

// SDL_DECLSPEC: A macro to tag a symbol as a public API.
// SDLCALL: A macro to set a function's calling conventions.

SDL_DECLSPEC Sint32 SDLCALL app_mod_handshake(Sint32 running_app_version) {
	// Plugin can bail out if it know that it doesn't meant to be used for app version below some release
	if ( running_app_version < APP_VERSION_TO_INT(0,1,12) ) return -1;
	// Plugin returns to app which version of app headers/libs it was compiled for, app decides to continue or not
	return hello(APP_VERSION_INT);
}

SDL_DECLSPEC Sint32 SDLCALL app_mod_init(void **userptr) {
	*userptr = SDL_malloc(42);
	if (!*userptr) return -1;
	return 0;
}

SDL_DECLSPEC Sint32 SDLCALL app_mod_fini(void *userptr) {
	SDL_free(userptr);
	return true;
}

// No app_mod_hook_purpose1 for this plugins (doesn't need it)

SDL_DECLSPEC Sint32 SDLCALL app_mod_hook_purpose2(void *userptr) {
	return 42;
}

Sint32 some_private_func(Sint32 a) {
	return a;
}
