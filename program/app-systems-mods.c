#include "app-systems-mods.h"

// ECS cached queries forward declarations
ECS_QUERY_DECLARE(ModReadyQuery);

// ECS Tasks forward declarations
void ModLookOnDisk(ecs_iter_t *it);

// ECS Systems forward declarations
void ModLoadFromDisk(ecs_iter_t *it);
void ModReloadFromDisk(ecs_iter_t *it);

// Utility functions forward declarations
SDL_EnumerationResult enumerate_mod_directory_callback(void *userdata, const char *dirname, const char *fname);
ecs_entity_t mod_tryload(ecs_world_t *world, ModOnDisk *d, ModInRAM *r);

//XXX make it part of world
char *mods_basepath = NULL;

void AppSystemsModsImport(ecs_world_t *world) {
    // https://www.flecs.dev/flecs/md_docs_2EntitiesComponents.html#registration
    // See the "modules" example
    ECS_MODULE(world, AppSystemsMods);
    ECS_IMPORT(world, AppComponentsMods);

    ECS_QUERY_DEFINE(world, ModReadyQuery,
        app.components.mods.ModOnDisk,
        app.components.mods.ModInRAM,
        (app.components.mods.ModState, app.components.mods.ModReady)
        );

    // The '()' means, don't match this component on an entity, while `[out]` indicates 
    // that the component is being written. This is interpreted by pipelines as a
    // system that can potentially enqueue commands for the ModInRAM component.
    ECS_SYSTEM(world, ModLoadFromDisk, EcsOnLoad,
        (app.components.mods.ModState, app.components.mods.ModAvailable),
        [in] app.components.mods.ModOnDisk,
        [out] app.components.mods.ModInRAM()
        );
    ECS_SYSTEM(world, ModReloadFromDisk, EcsOnLoad,
        (app.components.mods.ModState, app.components.mods.ModRunning),
        (app.components.mods.ModFlags, app.components.mods.ModReloadable),
        (app.components.mods.ModFlags, app.components.mods.ModNewerOnDisk),
        [in] app.components.mods.ModOnDisk,
        [out] app.components.mods.ModInRAM
        );

    // Periodic Tasks
    // ECS_SYSTEM(world, ModLookOnDisk, EcsOnLoad, 0); does not set .interval = ...
    /*ecs_entity_t ecs_id(ModLookOnDisk) = */ ecs_system(world, { /* ecs_system_desc_t */
        .entity = ecs_entity(world, { /* ecs_entity_desc_t */
            .name = "ModLookOnDisk",
            .add = ecs_ids( ecs_dependson(EcsOnLoad) )
        }),
        .callback = ModLookOnDisk,
        .interval = 1.0,
    });
}

// Systems run once per frame but only if entities that need attention are matching
void ModLoadFromDisk(ecs_iter_t *it) {
    ModOnDisk *d = ecs_field(it, ModOnDisk, 1);

    ModInRAM r;
    for (int i = 0; i < it->count; i++) {
        SDL_zero(r);
        ecs_entity_t next_state = mod_tryload(it->real_world, d+i, &r);
        ecs_entity_t mod = it->entities[i];
        ecs_set_ptr(it->world, mod, ModInRAM, &r);
        // Set mod state. As ModState is tagged Exclusive, add_pair will replace the previous pair
        ecs_add_pair(it->world, mod, ModState, next_state);
    }

}
void ModReloadFromDisk(ecs_iter_t *it) {
    app_info("%016"PRIu64" ModReloadFromDisk() begin", SDL_GetTicksNS());
}

// Task, run once per second TODO use libevent to watch folder
void ModLookOnDisk(ecs_iter_t *it) {
    if (!mods_basepath) { 
        if (!SDL_asprintf(&mods_basepath, APP_MOD_PATH_FROM_BASEPATH, SDL_GetBasePath())) {
            app_error("%016"PRIu64" ModLookOnDisk(): SDL_asprintf(&mods_basepath, ...) failed", SDL_GetTicksNS());
            return;
        }
    }

    if (!SDL_EnumerateDirectory(mods_basepath, enumerate_mod_directory_callback, it->world)) {
        app_error("%016"PRIu64" ModLookOnDisk(): SDL_EnumerateDirectory(%s) failed", SDL_GetTicksNS(), mods_basepath);
    }
}

// callback for each matching file/folder from SDL_EnumerateDirectory(mods_basepath, ...) in ModLookOnDisk(ecs_iter_t *it)
SDL_EnumerationResult enumerate_mod_directory_callback(void *userdata, const char *dirname, const char *fname) {
#define LOG_PREFIX "%016"PRIu64" enumerate_mod_directory_callback(world, \"%s\", \"%s\")"
    //app_trace(LOG_PREFIX, SDL_GetTicksNS(), dirname, fname);

    ecs_world_t *world = (ecs_world_t *)userdata;

    SDL_PathInfo info;
    char *mod_dirpath;
    if (!SDL_asprintf(&mod_dirpath, "%s%s", dirname, fname)) {
        app_error(LOG_PREFIX ": SDL_asprintf(&mod_dirpath, \"%%s%%s\",...): %s",
                SDL_GetTicksNS(), dirname, fname, SDL_GetError());
        goto bad4;
    }
    // Silently skip files living at *dirname
    if (!SDL_GetPathInfo(mod_dirpath, &info)) {
        app_error(LOG_PREFIX ": SDL_GetPathInfo(): %s", SDL_GetTicksNS(), dirname, fname, SDL_GetError());
        goto bad3;
    }
    if (info.type != SDL_PATHTYPE_DIRECTORY) {
        goto bad3;
    }
    char *so_path;
    if (!SDL_asprintf(&so_path, "%s/"APP_MOD_SUBDIR"%s"APP_MOD_FILEEXT, mod_dirpath, fname)) {
        app_error(LOG_PREFIX ": SDL_asprintf(...,\"%%s/APP_MOD_SUBDIR%%sAPP_MOD_FILEEXT\",...): %s",
                SDL_GetTicksNS(), dirname, fname, SDL_GetError());
        goto bad2;
    }
    // Skip if we can't stat the so file
    if (!SDL_GetPathInfo(so_path, &info)) {
        app_error(LOG_PREFIX ": %s:%s", SDL_GetTicksNS(), dirname, fname, so_path, SDL_GetError());
        goto bad2;
    }
    if (info.type != SDL_PATHTYPE_FILE) {
        app_info(LOG_PREFIX ": %s isn't a file", SDL_GetTicksNS(), dirname, fname, so_path);
        goto bad2;
    }
    // TODO see if we can PascalCase fname (without mod- prefix)
    char *mod_entity_name;
    if (!SDL_asprintf(&mod_entity_name, "mod.meta.%s", fname)) {
        app_error(LOG_PREFIX ": SDL_asprintf(&mod_entity_name, \"mods.%%s\",...): %s",
                SDL_GetTicksNS(), dirname, fname, SDL_GetError());
        goto bad2;
    }
    char *mod_name = SDL_strdup(fname);
    if (!mod_name) {
        app_error(LOG_PREFIX ": SDL_strdup(fname): %s",
                SDL_GetTicksNS(), dirname, fname, SDL_GetError());
        goto bad1;
    }

    // Create or refresh the current mod entity
    ecs_entity_t mod = ecs_entity(world, { .name = mod_entity_name });
    ecs_set(world, mod, ModOnDisk, { .name = mod_name, .so_path = so_path, .modify_time = info.modify_time });
    if (!ecs_has_pair(world, mod, ModState, EcsWildcard)) {
        ecs_add_pair(world, mod, ModState, ModAvailable);
    }

    SDL_free(mod_dirpath);
    return SDL_ENUM_CONTINUE;

    SDL_free(mod_name);
bad1:
    SDL_free(mod_entity_name);
bad2:
    SDL_free(so_path);
bad3:
    SDL_free(mod_dirpath);
bad4:
    // On failure path for this mod, we want to examine the next one
    return SDL_ENUM_CONTINUE;
#undef LOG_PREFIX
}

ecs_entity_t /* ModState */ mod_tryload(ecs_world_t *world, ModOnDisk *d, ModInRAM *r) {
    app_warn("%016"PRIu64" mod_tryload(): %s", SDL_GetTicksNS(), d->so_path);

    ecs_entity_t next_state = ModLoadFailed;

    r->shared_object = SDL_LoadObject(d->so_path);
    if (!r->shared_object) {
        app_warn("%016"PRIu64" mod_tryload(): SDL_LoadObject(): %s", SDL_GetTicksNS(), SDL_GetError());
        goto bad2;
    }

    mod_handshake_v1_t mod_handshake_v1 = (mod_handshake_v1_t) SDL_LoadFunction(r->shared_object, "mod_handshake_v1");
    if (!mod_handshake_v1) {
        app_warn("%016"PRIu64" mod_tryload(): SDL_LoadFunction(..., mod_handshake_v1): %s", SDL_GetTicksNS(), SDL_GetError());
        goto bad;
    }

    r->build_dep_version_compiled_against = mod_handshake_v1(APP_VERSION_INT);
    if (r->build_dep_version_compiled_against != BUILD_DEP_VERSION_INT) {
        app_warn("%016"PRIu64" %s: mod_handshake_v1(): Incompatible. Please update this mod. "
                "(Need to be compiled againt BUILD_DEP_VERSION %s",
                SDL_GetTicksNS(), d->name, BUILD_DEP_VERSION_STR);
        //TODO add returned version to msg, it needs to be converted from int to string
        next_state = ModIncompatible;
        goto bad;
    }

    r->mod_init_v1 = (mod_init_v1_t) SDL_LoadFunction(r->shared_object, "mod_init_v1");
    if (!r->mod_init_v1) {
        app_warn("%016"PRIu64" mod_tryload(): SDL_LoadFunction(..., mod_init_v1): %s", SDL_GetTicksNS(), SDL_GetError());
        goto bad;
    }

    r->mod_fini_v1 = (mod_fini_v1_t) SDL_LoadFunction(r->shared_object, "mod_fini_v1");
    if (!r->mod_fini_v1) {
        app_warn("%016"PRIu64" mod_tryload(): SDL_LoadFunction(..., mod_fini_v1): %s", SDL_GetTicksNS(), SDL_GetError());
        goto bad;
    }

    // Load all optionnal hook(s)
    r->mod_reload_v1 = (mod_reload_v1_t) SDL_LoadFunction(r->shared_object, "mod_reload_v1");

    next_state = ModReady;
    return next_state;

bad:
    SDL_UnloadObject(r->shared_object);
bad2:
    return next_state;
}

// Unregistered System (not declared as ECS_SYSTEM(...), because we want to run
// mod_init_v1() outside of ecs_progress(), see SDL_AppIterate() in sdl-app-iterate.c
void ModRunInit(ecs_iter_t *it) {
    ModOnDisk *d = ecs_field(it, ModOnDisk, 0);
    ModInRAM *r = ecs_field(it, ModInRAM, 1);

    for (int i = 0; i < it->count; i++) {
        void *userptr = NULL;
        app_warn("%016"PRIu64" ModRunInit() for %s: calling mod_init_v1(%p, %p) at %p",
                SDL_GetTicksNS(), d->name, it->world, &userptr, r[i].mod_init_v1);

        mod_result_t res = MOD_RESULT_INVALID;
        if ( r[i].mod_init_v1 ) {
            res = r[i].mod_init_v1(it->world, &userptr);
        }

        app_warn("%016"PRIu64" ModRunInit(): mod_init_v1() returned: %d",
                SDL_GetTicksNS(), res);

        ecs_entity_t next_state;
        switch (res) {
            case MOD_RESULT_SUCCESS:
                next_state = ModTerminated;
                break;
            case MOD_RESULT_CONTINUE:
                next_state = ModRunning;
                break;
            default: /* MOD_RESULT_FAILURE or unknown value */
                next_state = ModInitFailed;
                break;
        }
        ecs_entity_t mod = it->entities[i];
        if ( next_state == ModRunning ) {
            // Save userptr returned by mod_init_v1 for further mod API calls
            ModInRAM r2;
            SDL_memcpy(&r2, r+i, sizeof(r2)); // TODO see doc if copy is mandatory here
            r2.userptr = userptr;
            ecs_set_ptr(it->world, mod, ModInRAM, &r2);
        }
        // Set mod state. As ModState is tagged Exclusive, add_pair will replace the previous pair
        ecs_add_pair(it->world, mod, ModState, next_state);
    }
}
