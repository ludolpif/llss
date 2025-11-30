#pragma once

typedef Sint32 (*app_mod_handshake_t)(Sint32 running_app_version);
typedef SDL_AppResult (*app_mod_init_t)(appstate_t *appstate, void **userptr);
typedef SDL_AppResult (*app_mod_fini_t)(void *userptr);
typedef SDL_AppResult (*app_mod_hook_purpose1_t)(void *userptr);
typedef SDL_AppResult (*app_mod_hook_purpose2_t)(void *userptr);

