#define APP_COMPONENTS_CORE_IMPL
#include "app-components-core.h"

// Phases for pipelines
APP_API ecs_entity_t RenderingPreImGui, RenderingOnImGui, RenderingPostImGui;

// Elements for async IO state
APP_API ECS_TAG_DECLARE(IOState);
APP_API ECS_ENTITY_DECLARE(IOComplete);
APP_API ECS_ENTITY_DECLARE(IOFailure);
APP_API ECS_ENTITY_DECLARE(IOCanceled);

// Components not using ECS_STRUCT() in .h file (for custom type reflection)
APP_API ECS_COMPONENT_DECLARE(AppMemoryFuncs);
APP_API ECS_COMPONENT_DECLARE(AppSDLContext);
APP_API ECS_COMPONENT_DECLARE(AppImGuiContext);
APP_API ECS_COMPONENT_DECLARE(AsyncIOOutcome);
APP_API ECS_COMPONENT_DECLARE(AppDmonEvent);

APP_API void AppComponentsCoreImport(ecs_world_t *world) {
    // https://www.flecs.dev/flecs/md_docs_2EntitiesComponents.html#registration
    // See the "modules" example
    ECS_MODULE(world, AppComponentsCore);

    // Phases for pipelines
    RenderingPreImGui = ecs_entity_init(world, &(ecs_entity_desc_t){
            .name = "RenderingPreImGui",
            .add = ecs_ids(EcsPhase)
            });
    ecs_add_pair(world, RenderingPreImGui, EcsDependsOn, EcsPostUpdate);

    RenderingOnImGui = ecs_entity_init(world, &(ecs_entity_desc_t){
            .name = "RenderingOnImGui",
            .add = ecs_ids(EcsPhase)
            });
    ecs_add_pair(world, RenderingOnImGui, EcsDependsOn, RenderingPreImGui);

    RenderingPostImGui = ecs_entity_init(world, &(ecs_entity_desc_t){
            .name = "RenderingPostImGui",
            .add = ecs_ids(EcsPhase)
            });
    ecs_add_pair(world, RenderingPostImGui, EcsDependsOn, RenderingOnImGui);

    // Elements for async IO state
    ECS_TAG_DEFINE(world, IOState);
    // Register IOState as exclusive relationship. This ensures that an entity
    // can only belong to a single IOState.
    ecs_add_id(world, IOState, EcsExclusive);
    ECS_ENTITY_DEFINE(world, IOComplete);
    ECS_ENTITY_DEFINE(world, IOFailure);
    ECS_ENTITY_DEFINE(world, IOCanceled);

    // Using ECS_STRUCT + ECS_META_COMPONENT when possible, if struct of primivite types
    // See type names at flecs.h "Primitive type definitions" section
    ECS_META_COMPONENT(world, AppVersion);
    ECS_META_COMPONENT(world, AppMainTimingContext);

    ECS_COMPONENT_DEFINE(world, AppMemoryFuncs);
    ecs_struct(world, {
        .entity = ecs_id(AppMemoryFuncs),
        .members = {
            { .name = "sdl_malloc_func",  .type = ecs_id(ecs_uptr_t) },
            { .name = "sdl_calloc_func",  .type = ecs_id(ecs_uptr_t) },
            { .name = "sdl_realloc_func", .type = ecs_id(ecs_uptr_t) },
            { .name = "sdl_free_func",    .type = ecs_id(ecs_uptr_t) },
            { .name = "imgui_malloc_func",.type = ecs_id(ecs_uptr_t) },
            { .name = "imgui_free_func",  .type = ecs_id(ecs_uptr_t) },
            { .name = "imgui_allocator_functions_user_data", .type = ecs_id(ecs_uptr_t) },
        }
    });

    ECS_COMPONENT_DEFINE(world, AppSDLContext);
    ecs_struct(world, {
        .entity = ecs_id(AppSDLContext),
        .members = {
            { .name = "main_window",  .type = ecs_id(ecs_uptr_t) },
            { .name = "gpu_device",   .type = ecs_id(ecs_uptr_t) },
            { .name = "sdl_io_queue", .type = ecs_id(ecs_uptr_t) },
        }
    });

    ECS_COMPONENT_DEFINE(world, AppImGuiContext);
    ecs_struct(world, {
        .entity = ecs_id(AppImGuiContext),
        .members = {
            { .name = "imgui_context",  .type = ecs_id(ecs_uptr_t) },
            { .name = "imgui_io",   .type = ecs_id(ecs_uptr_t) },
        }
    });

    ECS_COMPONENT_DEFINE(world, AsyncIOOutcome);
    ecs_struct(world, {
        .entity = ecs_id(AsyncIOOutcome),
        .members = {
            { .name = "asyncio", .type = ecs_id(ecs_uptr_t) },
            { .name = "type", .type = ecs_id(ecs_i32_t) },
            { .name = "result", .type = ecs_id(ecs_i32_t) },
            { .name = "buffer", .type = ecs_id(ecs_uptr_t) },
            { .name = "offset", .type = ecs_id(ecs_u64_t) },
            { .name = "bytes_requested", .type = ecs_id(ecs_u64_t) },
            { .name = "bytes_transferred", .type = ecs_id(ecs_u64_t) },
            { .name = "userdata", .type = ecs_id(ecs_uptr_t) },
        }
    });
    ECS_COMPONENT_DEFINE(world, AppDmonEvent);
    ecs_struct(world, {
        .entity = ecs_id(AppDmonEvent),
        .members = {
            { .name = "watch_id", .type = ecs_id(ecs_u32_t) },
            { .name = "action", .type = ecs_id(ecs_i32_t) },
            { .name = "rootdir", .type = ecs_id(ecs_string_t) },
            { .name = "filepath", .type = ecs_id(ecs_string_t) },
            { .name = "oldfilepath", .type = ecs_id(ecs_string_t) },
            { .name = "user", .type = ecs_id(ecs_uptr_t) },
        }
    });
}
