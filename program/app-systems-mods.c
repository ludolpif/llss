#include "app-systems-mods.h"
#include "app-components-core.h"

// ECS cached queries forward declarations
ECS_QUERY_DECLARE(ModInitializableQuery);
ECS_QUERY_DECLARE(ModRunningNewerOnDiskQuery);
ECS_QUERY_DECLARE(ModTerminatingQuery);

//XXX make it part of world
char *mods_basepath = NULL;

void AppSystemsModsImport(ecs_world_t *world) {
    // https://www.flecs.dev/flecs/md_docs_2EntitiesComponents.html#registration
    // See the "modules" example
    ECS_MODULE(world, AppSystemsMods);
    ECS_IMPORT(world, AppComponentsMods);

    ECS_QUERY_DEFINE(world, ModInitializableQuery,
        [in] app.components.mods.ModOnDisk,
        [inout] app.components.mods.ModInRAM,
        (app.components.mods.ModState, app.components.mods.ModInitializable)
        );
    /*
     * FIXME this ModRunningNewerOnDiskQuery is for a ModRunningNewerOnDiskQuery system
     * that just make an "empty" transition without any conditions. Remove it but...
     * Scopes currently have the following limitations:
     * scopes can only be combined with Not operators (e.g. !{ ... }). Future versions of flecs
     * will add support for combining scopes with Or operators (e.g. { ... } || { ... }).
     */
    ECS_QUERY_DEFINE(world, ModRunningNewerOnDiskQuery,
        [in] app.components.mods.ModOnDisk,
        [inout] app.components.mods.ModInRAM,
        (app.components.mods.ModState, app.components.mods.ModRunning),
        (app.components.mods.ModFlags, app.components.mods.ModNewerOnDisk)
        );
    ECS_QUERY_DEFINE(world, ModTerminatingQuery,
        [in] app.components.mods.ModOnDisk,
        [inout] app.components.mods.ModInRAM,
        (app.components.mods.ModState, app.components.mods.ModInitFailed) ||
        (app.components.mods.ModState, app.components.mods.ModTerminating)
        );

    ECS_SYSTEM(world, ModPrepareFromDisk, EcsOnUpdate,
        [inout] app.components.mods.ModOnDisk,
        (app.components.mods.ModState, app.components.mods.ModAvailable)
        );
    ECS_SYSTEM(world, ModPrepareAgainFromDisk, EcsOnUpdate,
        [inout] app.components.mods.ModOnDisk,
        (app.components.mods.ModState, app.components.mods.ModIncompatible) ||
        (app.components.mods.ModState, app.components.mods.ModUnloaded) ||
        (app.components.mods.ModState, app.components.mods.ModLoadFailed),
        (app.components.mods.ModFlags, app.components.mods.ModNewerOnDisk)
        );
    // The '()' means, don't match this component on an entity, while `[out]` indicates
    // that the component is being written. This is interpreted by pipelines as a
    // system that can potentially enqueue commands for the ModInRAM component.
    ECS_SYSTEM(world, ModLoad, EcsOnUpdate,
        [in] app.components.mods.ModOnDisk,
        [inout] app.components.mods.ModInRAM(),
        (app.components.mods.ModState, app.components.mods.ModLoadable)
        );
#ifdef APP_MOD_COPYONLOAD
    ECS_SYSTEM(world, ModCopy, EcsOnUpdate,
        [in] app.components.mods.ModOnDisk,
        [in] app.components.core.AsyncIOOutcome,
        (app.components.core.IOState, app.components.core.IOComplete)
        );
#endif
    ECS_SYSTEM(world, ModUnload, EcsOnUpdate,
        [in] app.components.mods.ModOnDisk,
        [inout] app.components.mods.ModInRAM,
        (app.components.mods.ModState, app.components.mods.ModUnloadable)
        );

    // Periodic Tasks
    // ECS_SYSTEM(world, ModLookOnDisk, EcsOnUpdate, 0); does not set .interval = ...
    /*ecs_entity_t ecs_id(ModLookOnDisk) = */ ecs_system(world, { /* ecs_system_desc_t */
        .entity = ecs_entity(world, { /* ecs_entity_desc_t */
            .name = "ModLookOnDisk",
            .add = ecs_ids( ecs_dependson(EcsOnUpdate) )
        }),
        .callback = ModLookOnDisk,
        .interval = 1.0,
    });
}

// Systems run once per frame but only if entities that need attention are matching
void ModPrepareFromDisk(ecs_iter_t *it) {
    ModOnDisk *d = ecs_field(it, ModOnDisk, 0);

    for (int i = 0; i < it->count; i++) {
        app_debug("%016"PRIu64" ModPrepareFromDisk(%s)", SDL_GetTicksNS(), d[i].name);
        ecs_entity_t mod = it->entities[i];
        ecs_entity_t next_state;

        d[i].load_id++;

#ifdef APP_MOD_COPYONLOAD
        // Windows locks .dll files on disk if they are loaded in RAM, by design
        // We go through extra steps to load a copy of the DLL, allowing hot-reloading
        next_state = ModCopying;

        if ( d[i].so_realpath ) {
            app_debug("%016"PRIu64" ModPrepareFromDisk(%s): removing: %s",
                    SDL_GetTicksNS(), d[i].name, d[i].so_realpath);
            if (!SDL_RemovePath(d[i].so_realpath)) {
                app_warn("%016"PRIu64" ModPrepareFromDisk(%s): can't remove previous mod copy: %s",
                        SDL_GetTicksNS(), d[i].name, SDL_GetError());
            }
            SDL_free(d[i].so_realpath);
        }
        if (!SDL_asprintf(&d[i].so_realpath, "%s/"APP_MOD_SUBDIR"%03"PRIi32"-%s"APP_MOD_FILEEXT,
                    d[i].mod_dirpath, d[i].load_id, d[i].name)) {
            app_error("%016"PRIu64" ModPrepareFromDisk(%s): SDL_asprintf(&so_realpath,...): %s",
                    SDL_GetTicksNS(), d[i].name, SDL_GetError());
            next_state = ModLoadFailed;
            goto bailout;
        }
        SDL_PathInfo info;
        if (SDL_GetPathInfo(d[i].so_realpath, &info) && !SDL_RemovePath(d[i].so_realpath)) {
            app_error("%016"PRIu64" ModPrepareFromDisk(%s): can't remove so_realpath: %s",
                    SDL_GetTicksNS(), d[i].name, SDL_GetError());
            next_state = ModLoadFailed;
            goto bailout;
        }

        const AppSDLContext *app_sdl_context = ecs_singleton_get(it->world, AppSDLContext);
        if (!SDL_LoadFileAsync(d[i].so_path, app_sdl_context->sdl_io_queue, (void *)mod)) {
            app_error("%016"PRIu64" ModPrepareFromDisk(%s): so_path unreadable: %s",
                    SDL_GetTicksNS(), d[i].name, d[i].so_path);
        }
#else
        // For other OSes, nothing to do for now, .so or .dylib can be replaced freely
        next_state = ModLoadable;
        goto bailout; // To skip a warning in non windows-cases
#endif
bailout:
        // Set mod state. As ModState is tagged Exclusive, add_pair will replace the previous pair
        ecs_add_pair(it->world, mod, ModState, next_state);
    }
}

void ModPrepareAgainFromDisk(ecs_iter_t *it) {
    app_debug("%016"PRIu64" ModPrepareAgainFromDisk(): starting", SDL_GetTicksNS());
    ModPrepareFromDisk(it);
}

#ifdef APP_MOD_COPYONLOAD
void ModCopy(ecs_iter_t *it) {
    const ModOnDisk *d = ecs_field(it, ModOnDisk, 0);
    const AsyncIOOutcome *o = ecs_field(it, AsyncIOOutcome, 1);
    const AppSDLContext *app_sdl_context = ecs_singleton_get(it->world, AppSDLContext);
    for (int i = 0; i < it->count; i++) {
        ecs_entity_t mod = it->entities[i];
        switch ( o[i].type ) {
            case SDL_ASYNCIO_TASK_READ:
                app_debug("%016"PRIu64" ModCopy(%s): read event", SDL_GetTicksNS(), d[i].name);
                if ( o[i].bytes_transferred != o[i].bytes_requested ) {
                    app_error("%016"PRIu64" ModCopy(%s): partial read",
                            SDL_GetTicksNS(), d[i].name);
                    goto bailout;
                }
                // No SDL_CloseAsyncIO() because we came from SDL_LoadFileAsync() here
                SDL_AsyncIO *asyncio = SDL_AsyncIOFromFile(d[i].so_realpath, "w+");
                if (!asyncio) {
                    app_error("%016"PRIu64" ModCopy(%s): can't open for writing: %s",
                            SDL_GetTicksNS(), d[i].name, SDL_GetError());
                    goto bailout;
                }
                if (!SDL_WriteAsyncIO(asyncio, o[i].buffer, 0, o[i].bytes_transferred,
                            app_sdl_context->sdl_io_queue, (void *)mod)) {
                    app_error("%016"PRIu64" ModCopy(%s): can't write: %s",
                            SDL_GetTicksNS(), d[i].name, SDL_GetError());
                    goto bailout;
                }
                break;
            case SDL_ASYNCIO_TASK_WRITE:
                app_debug("%016"PRIu64" ModCopy(%s): write event", SDL_GetTicksNS(), d[i].name);
                if ( o[i].bytes_transferred != o[i].bytes_requested ) {
                    app_error("%016"PRIu64" ModCopy(%s): partial write",
                            SDL_GetTicksNS(), d[i].name);
                    goto bailout;
                }
                if (!SDL_CloseAsyncIO(o[i].asyncio, false,
                            app_sdl_context->sdl_io_queue, (void *)mod)) {
                    app_info("%016"PRIu64" ModCopy(%s): can't close after write: %s",
                            SDL_GetTicksNS(), d[i].name, SDL_GetError());
                }
                break;
            case SDL_ASYNCIO_TASK_CLOSE:
                app_info("%016"PRIu64" ModCopy(%s): close event", SDL_GetTicksNS(), d[i].name);
                // Set mod state. As ModState is tagged Exclusive, add_pair will replace the previous pair
                ecs_add_pair(it->world, mod, ModState, ModLoadable);
                break;
        }
bailout:
        ecs_remove_pair(it->world, mod, IOState, IOComplete);
    }
}
#endif /*APP_MOD_COPYONLOAD*/

void ModLoad(ecs_iter_t *it) {
    const ModOnDisk *d = ecs_field(it, ModOnDisk, 0);
    ModInRAM r;
    for (int i = 0; i < it->count; i++) {
        app_debug("%016"PRIu64" ModLoad(%s)", SDL_GetTicksNS(), d[i].name);
        ecs_entity_t next_state;

        SDL_zero(r);
        next_state = mod_tryload(d+i, &r);

        ecs_entity_t mod = it->entities[i];
        ecs_set_ptr(it->world, mod, ModInRAM, &r);

        ecs_remove_pair(it->world, mod, ModFlags, ModNewerOnDisk);
        // Set mod state. As ModState is tagged Exclusive, add_pair will replace the previous pair
        ecs_add_pair(it->world, mod, ModState, next_state);
    }
}

void ModUnload(ecs_iter_t *it) {
    const ModOnDisk *d = ecs_field(it, ModOnDisk, 0);
    ModInRAM *r = ecs_field(it, ModInRAM, 1);
    for (int i = 0; i < it->count; i++) {
        ecs_entity_t mod = it->entities[i];
        uint32_t flags = MOD_FLAGS_NONE;
        if ( ecs_has_pair(it->world, mod, ModFlags, ModNewerOnDisk) ) {
            flags |= MOD_FLAGS_RELOADING;
        }
        app_warn("%016"PRIu64" ModUnload(%s): calling SDL_UnloadObject()",
                SDL_GetTicksNS(), d[i].name);
        SDL_UnloadObject(r->shared_object);

        if ( !(flags & MOD_FLAGS_RELOADING) ) {
            r[i].userptr = NULL;
        }
        r[i].build_dep_version_compiled_against = 0;
        r[i].mod_init_v1 = NULL;
        r[i].mod_fini_v1 = NULL;
        r[i].shared_object = NULL;
        // Set mod state. As ModState is tagged Exclusive, add_pair will replace the previous pair
        ecs_add_pair(it->world, mod, ModState, ModUnloaded);
    }
}

// Unregistered System (not declared as ECS_SYSTEM(...), because we want to run
// mod_init_v1() outside of ecs_progress(), see SDL_AppIterate() in sdl-app-iterate.c
void ModInit(ecs_iter_t *it) {
    const ModOnDisk *d = ecs_field(it, ModOnDisk, 0);
    ModInRAM *r = ecs_field(it, ModInRAM, 1);

    for (int i = 0; i < it->count; i++) {
        mod_result_t res = MOD_RESULT_INVALID;
        uint32_t flags = MOD_FLAGS_NONE;
        if ( r[i].userptr ) {
            flags |= MOD_FLAGS_RELOADING;
        }
        if ( r[i].mod_init_v1 ) {
            app_warn("%016"PRIu64" ModInit(%s): calling mod_init_v1(world, %"PRIu32", %p)",
                    SDL_GetTicksNS(), d[i].name, flags, r[i].userptr);

            res = r[i].mod_init_v1(it->world, flags, &r[i].userptr);
        }
        app_warn("%016"PRIu64" ModInit(%s): mod_init_v1() returned: %d",
                SDL_GetTicksNS(), d[i].name, res);

        // Set mod state. As ModState is tagged Exclusive, add_pair will replace the previous pair
        ecs_entity_t next_state = (res==MOD_RESULT_SUCCESS)?ModRunning:ModInitFailed;
        ecs_entity_t mod = it->entities[i];
        ecs_add_pair(it->world, mod, ModState, next_state);

        // In case we are doing mod hot-reloading, we are done, remove the flag
        ecs_remove_pair(it->world, mod, ModFlags, ModNewerOnDisk);
    }
}

// Unregistered System (not declared as ECS_SYSTEM(...), same reason as for ModInit
void ModFini(ecs_iter_t *it) {
    const ModOnDisk *d = ecs_field(it, ModOnDisk, 0);
    ModInRAM *r = ecs_field(it, ModInRAM, 1);
    for (int i = 0; i < it->count; i++) {
        ecs_entity_t mod = it->entities[i];
        mod_result_t res = MOD_RESULT_INVALID;
        uint32_t flags = MOD_FLAGS_NONE;
        if ( ecs_has_pair(it->world, mod, ModFlags, ModNewerOnDisk) ) {
            flags |= MOD_FLAGS_RELOADING;
        }
        if ( r[i].mod_fini_v1 ) {
            app_warn("%016"PRIu64" ModFini(%s): calling mod_fini_v1(%"PRIu32", %p)",
                    SDL_GetTicksNS(), d[i].name, flags, r[i].userptr);
            res = r[i].mod_fini_v1(flags, r[i].userptr);
        }
        app_warn("%016"PRIu64" ModFini(%s): mod_fini_v1() returned: %d",
                SDL_GetTicksNS(), d[i].name, res);

        ecs_entity_t next_state = (res==MOD_RESULT_SUCCESS)?ModUnloadable:ModFiniFailed;

        // Set mod state. As ModState is tagged Exclusive, add_pair will replace the previous pair
        ecs_add_pair(it->world, mod, ModState, next_state);
    }
}

// Task, run once per second TODO use libevent to watch folder
void ModLookOnDisk(ecs_iter_t *it) {
    if (!mods_basepath) {
        if (!SDL_asprintf(&mods_basepath, APP_MOD_PATH_FROM_BASEPATH, SDL_GetBasePath())) {
            app_error("%016"PRIu64" ModLookOnDisk(): SDL_asprintf(&mods_basepath, ...) failed",
                    SDL_GetTicksNS());
            return;
        }
    }

    if (!SDL_EnumerateDirectory(mods_basepath, enumerate_mod_directory_callback, it->world)) {
        app_error("%016"PRIu64" ModLookOnDisk(): SDL_EnumerateDirectory(%s) failed",
                SDL_GetTicksNS(), mods_basepath);
    }
}

// callback for each matching file/folder from above SDL_EnumerateDirectory(mods_basepath, ...)
SDL_EnumerationResult enumerate_mod_directory_callback(void *userdata, const char *dirname, const char *fname) {
#define LOG_PREFIX "%016"PRIu64" enumerate_mod_directory_callback(world, \"%s\", \"%s\")"
    //app_trace(LOG_PREFIX, SDL_GetTicksNS(), dirname, fname);

    ecs_world_t *world = (ecs_world_t *)userdata;

    SDL_PathInfo info;
    char *mod_dirpath = NULL;
    char *so_path = NULL;
    char *mod_entity_name = NULL;
    char *mod_name = NULL;
    char *so_realpath = NULL;

    // Silently skip mod-template
    if ( SDL_strcmp("mod-template", fname)==0 ) {
        goto bailout;
    }
    if (!SDL_asprintf(&mod_dirpath, "%s%s", dirname, fname)) {
        app_error(LOG_PREFIX ": SDL_asprintf(&mod_dirpath, \"%%s%%s\",...): %s",
                SDL_GetTicksNS(), dirname, fname, SDL_GetError());
        goto bailout;
    }
    // Silently skip files living at *dirname
    if (!SDL_GetPathInfo(mod_dirpath, &info)) {
        app_error(LOG_PREFIX ": SDL_GetPathInfo(): %s",
                SDL_GetTicksNS(), dirname, fname, SDL_GetError());
        goto bailout;
    }
    if (info.type != SDL_PATHTYPE_DIRECTORY) {
        goto bailout;
    }
    // Generate full path
    if (!SDL_asprintf(&so_path, "%s/"APP_MOD_SUBDIR"%s"APP_MOD_FILEEXT, mod_dirpath, fname)) {
        app_error(LOG_PREFIX ": SDL_asprintf(&so_path,...): %s",
                SDL_GetTicksNS(), dirname, fname, SDL_GetError());
        goto bailout;
    }
    // Skip if we can't stat the so file
    if (!SDL_GetPathInfo(so_path, &info)) {
        app_error(LOG_PREFIX ": %s:%s", SDL_GetTicksNS(), dirname, fname, so_path, SDL_GetError());
        goto bailout;
    }
    if (info.type != SDL_PATHTYPE_FILE) {
        app_info(LOG_PREFIX ": %s isn't a file", SDL_GetTicksNS(), dirname, fname, so_path);
        goto bailout;
    }
    // Prepare some strings for the ModOnDisk component
    //FIXME This seems to be a bad method, at least in Explorer parent entities keep blinking
    if (!SDL_asprintf(&mod_entity_name, "mod.meta.%s", fname)) {
        app_error(LOG_PREFIX ": SDL_asprintf(&mod_entity_name, ...): %s",
                SDL_GetTicksNS(), dirname, fname, SDL_GetError());
        goto bailout;
    }
    mod_name = SDL_strdup(fname);
    if (!mod_name) {
        app_error(LOG_PREFIX ": SDL_strdup(fname): %s",
                SDL_GetTicksNS(), dirname, fname, SDL_GetError());
        goto bailout;
    }
    // Create or refresh the current mod entity
    ecs_entity_t mod = ecs_entity(world, { .name = mod_entity_name });
    const ModOnDisk *d = ecs_get(world, mod, ModOnDisk);
    ecs_i32_t load_id = 0;
    if ( d ) {
        load_id = d->load_id;
        so_realpath = d->so_realpath;
    }
#ifndef APP_MOD_COPYONLOAD
    // On all non-windows platform we directly load the shared object
    // On Windows, more steps are required as an open .dll is a locked file on disk by design
    if (!so_realpath) {
        so_realpath = SDL_strdup(so_path);
        if (!so_realpath) {
            app_error(LOG_PREFIX ": SDL_strdup(so_path): %s",
                    SDL_GetTicksNS(), dirname, fname, SDL_GetError());
            goto bailout;
        }
    }
#endif
    ecs_set(world, mod, ModOnDisk, {
        .name = mod_name,
        .mod_dirpath = mod_dirpath,
        .so_path = so_path,
        .so_realpath = so_realpath,
        .load_id = load_id,
        .modify_time = info.modify_time
    });
    if (!ecs_has_pair(world, mod, ModState, EcsWildcard)) {
        ecs_add_pair(world, mod, ModState, ModAvailable);
    }
    // If the mod entity was refreshed and not just created, and module was previously loaded,
    // this entity have already a ModInRAM component
    const ModInRAM *r = ecs_get(world, mod, ModInRAM);
    if (r && info.modify_time > r->modify_time_when_loaded &&
            !ecs_has_pair(world, mod, ModFlags, ModNewerOnDisk)) {
        app_info(LOG_PREFIX ": mod has changed on disk (%"PRIu64" > %"PRIu64")",
                SDL_GetTicksNS(), "...", fname, info.modify_time, r->modify_time_when_loaded);
        ecs_add_pair(world, mod, ModFlags, ModNewerOnDisk);
    }

    return SDL_ENUM_CONTINUE;

bailout:
    SDL_free(mod_name);
    SDL_free(mod_entity_name);
    SDL_free(so_path);
    SDL_free(mod_dirpath);

    // On failure path for this mod, we want to examine the next one
    return SDL_ENUM_CONTINUE;
#undef LOG_PREFIX
}

// Bare C function, not a system
ecs_entity_t /* ModState */ mod_tryload(const ModOnDisk *d, ModInRAM *r) {
    r->shared_object = SDL_LoadObject(d->so_realpath);
    if (!r->shared_object) {
        app_warn("%016"PRIu64" mod_tryload(): SDL_LoadObject(): %s",
                SDL_GetTicksNS(), SDL_GetError());
        return ModLoadFailed;
    }
    r->modify_time_when_loaded = d->modify_time;

    // load and run mod_handshake_v1()
    mod_handshake_v1_t mod_handshake_v1 =
        (mod_handshake_v1_t) SDL_LoadFunction(r->shared_object, "mod_handshake_v1");
    if (!mod_handshake_v1) {
        app_warn("%016"PRIu64" mod_tryload(): SDL_LoadFunction(..., mod_handshake_v1): %s",
                SDL_GetTicksNS(), SDL_GetError());
        SDL_UnloadObject(r->shared_object);
        return ModLoadFailed;
    }

    int32_t res = mod_handshake_v1(APP_VERSION_INT);
    if (res != BUILD_DEP_VERSION_INT) {
        if ( res == -1 ) {
            app_warn("%016"PRIu64" %s: mod_handshake_v1(): Incompatible. Please update the app.",
                    SDL_GetTicksNS(), d->name);
        } else {
            app_warn("%016"PRIu64" %s: mod_handshake_v1(): Incompatible. Please update this mod. "
                    "(Need to be compiled againt BUILD_DEP_VERSION %s)",
                    SDL_GetTicksNS(), d->name, BUILD_DEP_VERSION_STR);
        }
        //TODO add returned version to msg, it needs to be converted from int to string
        SDL_UnloadObject(r->shared_object);
        return ModIncompatible;
    }
    r->build_dep_version_compiled_against = res;

    // load mod_init_v1(), don't run it now, world is read-only and mod is allowed to ECS_IMPORT()
    r->mod_init_v1 = (mod_init_v1_t) SDL_LoadFunction(r->shared_object, "mod_init_v1");
    if (!r->mod_init_v1) {
        app_warn("%016"PRIu64" mod_tryload(): SDL_LoadFunction(..., mod_init_v1): %s",
                SDL_GetTicksNS(), SDL_GetError());
        SDL_UnloadObject(r->shared_object);
        return ModLoadFailed;
    }

    // load mod_fini_v1()
    r->mod_fini_v1 = (mod_fini_v1_t) SDL_LoadFunction(r->shared_object, "mod_fini_v1");
    if (!r->mod_fini_v1) {
        app_warn("%016"PRIu64" mod_tryload(): SDL_LoadFunction(..., mod_fini_v1): %s",
                SDL_GetTicksNS(), SDL_GetError());
        SDL_UnloadObject(r->shared_object);
        return ModLoadFailed;
    }

    return ModInitializable;
}
