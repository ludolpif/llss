#define APP_COMPONENTS_MODS_IMPL
#include "app-components-mods.h"

void AppComponentsModsImport(ecs_world_t *world) {
    // https://www.flecs.dev/flecs/md_docs_2EntitiesComponents.html#registration
    // See the "modules" example
    ECS_MODULE(world, AppComponentsMods);

    ECS_TAG_DEFINE(world, ModState);
    // Register ModState as exclusive relationship. This ensures that an entity
    // can only belong to a single ModState.
    ecs_add_id(world, ModState, EcsExclusive);

    ECS_ENTITY_DEFINE(world, ModAvailable);
    ECS_ENTITY_DEFINE(world, ModCopying);
    ECS_ENTITY_DEFINE(world, ModLoadable);
    ECS_ENTITY_DEFINE(world, ModLoadFailed);
    ECS_ENTITY_DEFINE(world, ModIncompatible);
    ECS_ENTITY_DEFINE(world, ModInitializable);
    ECS_ENTITY_DEFINE(world, ModInitFailed);
    ECS_ENTITY_DEFINE(world, ModRunning);
    ECS_ENTITY_DEFINE(world, ModTerminating);
    ECS_ENTITY_DEFINE(world, ModUnloadable);
    ECS_ENTITY_DEFINE(world, ModUnloaded);

    // ModFlags is not an exclusive relationship.
    ECS_TAG_DEFINE(world, ModFlags);
    ECS_ENTITY_DEFINE(world, ModNewerOnDisk);

    ECS_TAG_DEFINE(world, ModIOAsync);

    // Components
    ECS_COMPONENT_DEFINE(world, ModOnDisk);
    ecs_struct(world, {
        .entity = ecs_id(ModOnDisk),
        .members = {
            { .name = "name",        .type = ecs_id(ecs_string_t) },
            { .name = "mod_dirpath", .type = ecs_id(ecs_string_t) },
            { .name = "so_path",     .type = ecs_id(ecs_string_t) },
            { .name = "so_realpath", .type = ecs_id(ecs_string_t) },
            { .name = "modify_time", .type = ecs_id(ecs_i64_t) },
            { .name = "load_id",     .type = ecs_id(ecs_i32_t) },
        }
    });

    ECS_COMPONENT_DEFINE(world, ModInRAM);
    ecs_struct(world, {
        .entity = ecs_id(ModInRAM),
        .members = {
            { .name = "shared_object",  .type = ecs_id(ecs_uptr_t) },
            { .name = "userptr",        .type = ecs_id(ecs_uptr_t) },
            { .name = "build_dep_version_compiled_against", .type = ecs_id(ecs_i32_t) },
            { .name = "modify_time_when_loaded", .type = ecs_id(ecs_i64_t) },
            { .name = "mod_init_v1",     .type = ecs_id(ecs_uptr_t) },
            { .name = "mod_fini_v1",     .type = ecs_id(ecs_uptr_t) },
        }
    });
}
