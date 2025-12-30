#include "app.h" // No "ecs-mods-lifecycle.h", embeded in app.h

#define APP_MOD_PATH_FROM_BASEPATH "%s../../../mods/"

#ifdef _DEBUG
#define APP_MOD_SUBDIR "program/x64/Debug/"
#else
#define APP_MOD_SUBDIR "program/x64/Release/"
#endif

#if defined(SDL_PLATFORM_WINDOWS)
#define APP_MOD_FILEEXT ".dll"
#elif defined(SDL_PLATFORM_APPLE)
#define APP_MOD_FILEEXT ".dylib"
#else
#define APP_MOD_FILEEXT ".so"
#endif

SDL_DECLSPEC ECS_TAG_DECLARE(ModState);
SDL_DECLSPEC ECS_ENTITY_DECLARE(Available);
SDL_DECLSPEC ECS_ENTITY_DECLARE(Incompatible);
SDL_DECLSPEC ECS_ENTITY_DECLARE(Loaded);
SDL_DECLSPEC ECS_ENTITY_DECLARE(LoadFailed);
SDL_DECLSPEC ECS_ENTITY_DECLARE(InitFailed);
SDL_DECLSPEC ECS_ENTITY_DECLARE(Running);
SDL_DECLSPEC ECS_ENTITY_DECLARE(Terminated);

SDL_DECLSPEC ECS_TAG_DECLARE(ModFlags);
SDL_DECLSPEC ECS_ENTITY_DECLARE(Reloadable);
SDL_DECLSPEC ECS_ENTITY_DECLARE(NewerOnDisk);

SDL_DECLSPEC ECS_COMPONENT_DECLARE(ModOnDisk);
SDL_DECLSPEC ECS_COMPONENT_DECLARE(ModInRAM);

// ECS Tasks forward declarations
SDL_DECLSPEC void ModLookOnDisk(ecs_iter_t *it);

// ECS Systems forward declarations
SDL_DECLSPEC void ModLoadFromDisk(ecs_iter_t *it);
SDL_DECLSPEC void ModReloadFromDisk(ecs_iter_t *it);

// Utility functions forward declarations
ecs_entity_t mod_tryload(ecs_world_t *world, ModOnDisk *d, ModInRAM *r);

void ModsLifecycleImport(ecs_world_t *world) {
	// https://www.flecs.dev/flecs/md_docs_2EntitiesComponents.html#registration
	// See the "modules" example
	ECS_MODULE(world, ModsLifecycle);

	ECS_TAG_DEFINE(world, ModState);
	// Register ModState as exclusive relationship. This ensures that an entity
	// can only belong to a single ModState.
	ecs_add_id(world, ModState, EcsExclusive);

	ECS_ENTITY_DEFINE(world, Available);
	ECS_ENTITY_DEFINE(world, Incompatible);
	ECS_ENTITY_DEFINE(world, LoadFailed);
	ECS_ENTITY_DEFINE(world, InitFailed);
	ECS_ENTITY_DEFINE(world, Running);
	ECS_ENTITY_DEFINE(world, Terminated);

	// ModFlags is not an exclusive relationship.
	ECS_TAG_DEFINE(world, ModFlags);
	ECS_ENTITY_DEFINE(world, Reloadable);
	ECS_ENTITY_DEFINE(world, NewerOnDisk);

	ECS_COMPONENT_DEFINE(world, ModOnDisk);
	ECS_COMPONENT_DEFINE(world, ModInRAM);

	// The '()' means, don't match this component on an entity, while `[out]` indicates 
	// that the component is being written. This is interpreted by pipelines as a
	// system that can potentially enqueue commands for the ModInRAM component.
	ECS_SYSTEM(world, ModLoadFromDisk, EcsPreUpdate, (ModState, Available), [in] ModOnDisk, [out] ModInRAM() );
	ECS_SYSTEM(world, ModReloadFromDisk, EcsPreUpdate, (ModState, Running), (ModFlags, Reloadable), (ModFlags, NewerOnDisk), [in] ModOnDisk, [out] ModInRAM);

	// Periodic Tasks
	// ECS_SYSTEM(world, ModLookOnDisk, EcsPreUpdate, 0); does not set .interval = ...
	/*ecs_entity_t ecs_id(ModLookOnDisk) = */ ecs_system(world, { /* ecs_system_desc_t */
		.entity = ecs_entity(world, { /* ecs_entity_desc_t */
			.name = "ModLookOnDisk",
			.add = ecs_ids( ecs_dependson(EcsPreUpdate) )
		}),
		.callback = ModLookOnDisk,
		.interval = 1.0,
	});
}

// Systems run once per frame but only if entities that need attention are matching
void ModLoadFromDisk(ecs_iter_t *it) {
	app_info("%016"PRIu64" ModLoadFromDisk() begin", SDL_GetTicksNS());
	ModOnDisk *d = ecs_field(it, ModOnDisk, 1);

	for (int i = 0; i < it->count; i++) {
		ModInRAM r = {};
		ecs_entity_t next_state = mod_tryload(it->world, d, &r);
		ecs_entity_t mod = it->entities[i];
		ecs_set_ptr(it->world, mod, ModInRAM, &r);
		ecs_add_pair(it->world, mod, ModState, next_state);
	}

}
void ModReloadFromDisk(ecs_iter_t *it) {
	app_info("%016"PRIu64" ModReloadFromDisk() begin", SDL_GetTicksNS());
}

// Task, run once per second
void ModLookOnDisk(ecs_iter_t *it) {
	app_info("%016"PRIu64" ModLookOnDisk() begin", SDL_GetTicksNS());

/*
	SDL_PathInfo info;
	char *mod_dirpath;
	// Note: SDL invite us to use "/" as path separator even on Windows, see https://github.com/libsdl-org/SDL/issues/11370
	if (!SDL_asprintf(&mod_dirpath, "%s%s", mods_basepath, name)) {
		app_error("%016"PRIu64" mod_tryload(appstate, \"%s\", \"%s\"): SDL_asprintf(...,\"%%s%%s\",...): %s",
				SDL_GetTicksNS(), mods_basepath, name, SDL_GetError());
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

	// Allocate and set mods->name[i]
	mods->name[i] = SDL_strdup(name);

	// Allocate and set mods->so_path[i]
	if (!SDL_asprintf(&mods->so_path[i], "%s%s/"APP_MOD_SUBDIR"%s"APP_MOD_FILEEXT,
			       	mods_basepath, name, name)) {
		app_error("%016"PRIu64" mod_tryload(): mod_tryload(appstate, \"%s\", \"%s\"): SDL_asprintf(...,\"%%s%%s/%%s%%s\",...): %s",
				SDL_GetTicksNS(), mods_basepath, name, SDL_GetError());
		goto bad4;
	}
	SDL_free(mods->so_path[i]);
	mods->so_path[i] = NULL;
	SDL_free(mods->name[i]);
	mods->name[i] = NULL;
bad3:
bad4:
		Incompatible, Loaded, Running
	// We want to skip this module but not prevent tryload of the next one

	SDL_free(mod_dirpath);
*/

	ecs_entity_t mod = ecs_entity(it->world, { .name = "ModThemeRlyeh3" });
	ecs_set(it->world, mod, ModOnDisk, {
			.name = "mod-theme-rlyeh3",
			.so_path = "/home/ludolpif/git/llss/mods/mod-theme-rlyeh3/program/x64/Debug/mod-theme-rlyeh3.so",
			});
	if (!ecs_has_pair(it->world, mod, ModState, EcsWildcard)) {
		ecs_add_pair(it->world, mod, ModState, Available);
	}
}

ecs_entity_t /* ModState */ mod_tryload(ecs_world_t *world, ModOnDisk *d, ModInRAM *r) {
	app_warn("%016"PRIu64" mod_tryload(): %s", SDL_GetTicksNS(), d->so_path);

	ecs_entity_t next_state = LoadFailed;

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
		next_state = Incompatible;
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

	// Optionnal Hook
	r->mod_reload_v1 = (mod_reload_v1_t) SDL_LoadFunction(r->shared_object, "mod_reload_v1");

	// Actually run mod_init_v1
	next_state = r->mod_init_v1(world, &r->userptr);
	if ( next_state == Incompatible ) {
			app_warn("%016"PRIu64" mod_tryload(): mod_init_v1() returned: Incompatible", SDL_GetTicksNS());
	} else if ( next_state != Running ) {
			app_warn("%016"PRIu64" mod_tryload(): mod_init_v1() failed", SDL_GetTicksNS());
			next_state = InitFailed;
	}

/* TODO this became systems tagged somewhere? What about execution order or precedences ?
	hook_ui_config_v1_t hook;
	hook_ui_main_v1_t hook;
	hook_ui_menu_v1_t hook;
*/
	return next_state;

bad:
	SDL_UnloadObject(r->shared_object);
bad2:
	return next_state;
}
/*
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

SDL_EnumerationResult mod_tryload(void *_appstate, const char *mods_basepath, const char *mod_dirname) { }
*/
