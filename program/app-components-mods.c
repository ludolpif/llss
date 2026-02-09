/*
 * This file is part of LLSS.
 *
 * LLSS is free software: you can redistribute it and/or modify it under the terms of the
 * Affero GNU General Public License as published by the Free Software Foundation,
 * either version 3 of the License, or (at your option) any later version.
 *
 * LLSS is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with LLSS.
 * If not, see <https://www.gnu.org/licenses/>. See LICENSE file at root of this git repo.
 *
 * Copyright 2025 ludolpif <ludolpif@gmail.com>
 */
#define APP_COMPONENTS_MODS_IMPL
#include "app-components-mods.h"

/* Boring but important memory management hooks */
ECS_DTOR(ModOnDisk, ptr, {
    //ecs_trace(__PRETTY_FUNCTION__);
    ecs_os_free(ptr->name);
    ecs_os_free(ptr->mod_dirpath);
    ecs_os_free(ptr->so_path);
    ecs_os_free(ptr->so_realpath);
})

ECS_MOVE(ModOnDisk, dst, src, {
    //ecs_trace(__PRETTY_FUNCTION__);
    ecs_os_free(dst->name);
    ecs_os_free(dst->mod_dirpath);
    ecs_os_free(dst->so_path);
    ecs_os_free(dst->so_realpath);
    SDL_copyp(dst, src);
    SDL_zerop(src); // mandatory to not have double-free, dtor still called after move
})

ECS_COPY(ModOnDisk, dst, src, {
    //ecs_trace(__PRETTY_FUNCTION__);
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

/* Import */
void AppComponentsModsImport(ecs_world_t *world) {
    // https://www.flecs.dev/flecs/md_docs_2EntitiesComponents.html#registration
    // See the "modules" example
    ECS_MODULE(world, AppComponentsMods);

    /* Elements for states represented as exclusive relationships */
    ECS_TAG_DEFINE(world, ModState);
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
    ECS_ENTITY_DEFINE(world, ModFiniFailed);
    ECS_ENTITY_DEFINE(world, ModUnloadable);
    ECS_ENTITY_DEFINE(world, ModUnloaded);

    /* Elements for non-exclusive relationships */
    ECS_TAG_DEFINE(world, ModFlags);
    ECS_ENTITY_DEFINE(world, ModNewerOnDisk);

    ECS_TAG_DEFINE(world, ModIOAsync);

    /* Component definitions, with metadata for REST API */
    ECS_META_COMPONENT(world, ModOnDisk);

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

    /* Boring but important memory management hooks */
    ecs_set_hooks(world, ModOnDisk, {
        //.ctor = ecs_ctor(ModOnDisk),
        .move = ecs_move(ModOnDisk),
        .copy = ecs_copy(ModOnDisk),
        .dtor = ecs_dtor(ModOnDisk),
    });
}
