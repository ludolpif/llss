#define APP_COMPONENTS_COMPOSITION_IMPL
#include "app-components-composition.h"

void AppComponentsCompositionImport(ecs_world_t *world) {
    // https://www.flecs.dev/flecs/md_docs_2EntitiesComponents.html#registration
    // See the "modules" example
    ECS_MODULE(world, AppComponentsComposition);

    ECS_META_COMPONENT(world, LayoutVariant);

    ECS_QUERY_DEFINE(world, ActivateLayoutVariantsQuery,
        [in] LayoutVariant,
        );
}
