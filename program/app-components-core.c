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
    ECS_META_COMPONENT(world, AppVersion);
    ECS_COMPONENT_DEFINE(world, AppMemoryFuncs);
    ECS_COMPONENT_DEFINE(world, AppSDLContext);
    ECS_COMPONENT_DEFINE(world, AppImGuiContext);
    ECS_META_COMPONENT(world, AppMainTimingContext);

}

