#define APP_COMPONENTS_CORE_IMPL
#include "app-components-core.h"

// Phases for pipelines
APP_API ecs_entity_t RenderingPreImGui, RenderingOnImGui, RenderingPostImGui;

APP_API ECS_COMPONENT_DECLARE(AppMemoryFuncs);
APP_API ECS_COMPONENT_DECLARE(AppSDLContext);
APP_API ECS_COMPONENT_DECLARE(AppImGuiContext);

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
}
