#include "mods-components-core-lifecycle.h"

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

void ModsSystemsCoreLifecycleImport(ecs_world_t *world) {
	// https://www.flecs.dev/flecs/md_docs_2EntitiesComponents.html#registration
	// See the "modules" example
	ECS_MODULE(world, ModsSystemsCoreLifecycle);
	ECS_IMPORT(world, ModsComponentsCoreLifecycle);

	// The '()' means, don't match this component on an entity, while `[out]` indicates 
	// that the component is being written. This is interpreted by pipelines as a
	// system that can potentially enqueue commands for the ModInRAM component.
	ECS_SYSTEM(world, ModLoadFromDisk, EcsOnLoad,
		(mods.components.core.lifecycle.ModState, mods.components.core.lifecycle.ModAvailable),
		[in] mods.components.core.lifecycle.ModOnDisk,
		[out] mods.components.core.lifecycle.ModInRAM()
		);
	ECS_SYSTEM(world, ModReloadFromDisk, EcsOnLoad,
		(mods.components.core.lifecycle.ModState, mods.components.core.lifecycle.ModRunning),
		(mods.components.core.lifecycle.ModFlags, mods.components.core.lifecycle.ModReloadable),
		(mods.components.core.lifecycle.ModFlags, mods.components.core.lifecycle.ModNewerOnDisk),
		[in] mods.components.core.lifecycle.ModOnDisk,
		[out] mods.components.core.lifecycle.ModInRAM
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
		ecs_entity_t next_state = mod_tryload(it->world, d+i, &r);
		ecs_entity_t mod = it->entities[i];
		ecs_set_ptr(it->world, mod, ModInRAM, &r);
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
	char *mod_entity_name;
	if (!SDL_asprintf(&mod_entity_name, "mods.%s", fname)) {
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

	// Optionnal Hook
	r->mod_reload_v1 = (mod_reload_v1_t) SDL_LoadFunction(r->shared_object, "mod_reload_v1");

	// Actually run mod_init_v1
	mod_result_t res = r->mod_init_v1(world, &r->userptr);
	
	if ( res != MOD_RESULT_CONTINUE ) {
		app_warn("%016"PRIu64" mod_tryload(): mod_init_v1() returned: %d", SDL_GetTicksNS(), res);
		next_state = ModInitFailed;
	} else {
		r->so_file_modify_time_when_loaded_in_ram = d->modify_time;
		next_state = ModRunning;
	}

bad:
	SDL_UnloadObject(r->shared_object);
bad2:
	return next_state;
}
