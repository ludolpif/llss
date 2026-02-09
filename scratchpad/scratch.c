#include "scratch.h"

ECS_STRUCT(ModOnDiskDraft, {
    char *name;
    char *mod_dirpath;
    char *so_path;
    char *so_realpath;
    SDL_Time modify_time;
    int32_t load_id;
});

ECS_DTOR(ModOnDiskDraft, ptr, {
    ecs_trace(__PRETTY_FUNCTION__);
    ecs_os_free(ptr->name);
    ecs_os_free(ptr->mod_dirpath);
    ecs_os_free(ptr->so_path);
    ecs_os_free(ptr->so_realpath);
})

ECS_MOVE(ModOnDiskDraft, dst, src, {
    ecs_trace(__PRETTY_FUNCTION__);
    ecs_os_free(dst->name);
    ecs_os_free(dst->mod_dirpath);
    ecs_os_free(dst->so_path);
    ecs_os_free(dst->so_realpath);
    SDL_copyp(dst, src);
    SDL_zerop(src); // mandatory to not have double-free, dtor still called after move
})

ECS_COPY(ModOnDiskDraft, dst, src, {
    ecs_trace(__PRETTY_FUNCTION__);
    ecs_os_free(dst->name);
    ecs_os_free(dst->mod_dirpath);
    ecs_os_free(dst->so_path);
    ecs_os_free(dst->so_realpath);
    dst->name = ecs_os_strdup(src->name);
    dst->mod_dirpath = ecs_os_strdup(src->mod_dirpath);
    dst->so_path = ecs_os_strdup(src->so_path);
    dst->so_realpath = ecs_os_strdup(src->so_realpath);
    dst->modify_time = src->modify_time;
    dst->load_id = src->load_id;
})

void init_hook(ecs_world_t *world) {
    ecs_primitive_init(world, &(ecs_primitive_desc_t){
            .entity = ecs_entity(world, { .name = "SDL_Time" })
            , .kind = EcsI64 }
            );

    ECS_META_COMPONENT(world, ModOnDiskDraft);
    ecs_set_hooks(world, ModOnDiskDraft, {
        .ctor = flecs_default_ctor,
        .move = ecs_move(ModOnDiskDraft),
        .copy = ecs_copy(ModOnDiskDraft),
        .dtor = ecs_dtor(ModOnDiskDraft),
    });
}

const char * fname = "mod";

bool tick_hook(ecs_world_t *world) {
    static ecs_i32_t load_id = 0;


    ecs_i64_t modify_time = SDL_GetTicksNS();
    char *so_path = NULL;
    char *mod_dirpath = NULL;
    char *so_realpath = NULL;

    mod_dirpath = SDL_strdup("./somedir/");
    SDL_asprintf(&so_path, "%s%s", mod_dirpath, "somefile.so");
    so_realpath = SDL_strdup(so_path);

    ecs_entity_t mod = ecs_entity(world, { .name = fname });
    ecs_set(world, mod, ModOnDiskDraft, {
        .name = "mod-hello",
        .mod_dirpath = mod_dirpath,
        .so_path = so_path,
        .so_realpath = so_path, //so_realpath,
        .load_id = load_id,
        .modify_time = modify_time
    });

    SDL_free(mod_dirpath);
    SDL_free(so_path);
    SDL_free(so_realpath);

    const ModOnDiskDraft *d = ecs_get(world, mod, ModOnDiskDraft);
    ecs_trace(".name: %s, .mod_dirpath: %s, .so_path: %s, .so_realpath: %s, .load_id: %d, .modify_time: %ld",
            d->name, d->mod_dirpath, d->so_path, d->so_realpath, d->load_id, d->modify_time);

    load_id++;
    return load_id < 5;
}

void fini_hook(ecs_world_t *world) {
    ecs_entity_t mod = ecs_entity(world, { .name = fname });
    ecs_delete(world, mod);
}
