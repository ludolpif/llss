#define MODS_COMPONENTS_CORE_LIFECYCLE_IMPL
#include "mods-components-core-lifecycle.h"

APP_API ECS_TAG_DECLARE(ModState);
APP_API ECS_ENTITY_DECLARE(ModAvailable);
APP_API ECS_ENTITY_DECLARE(ModIncompatible);
APP_API ECS_ENTITY_DECLARE(ModLoadFailed);
APP_API ECS_ENTITY_DECLARE(ModInitFailed);
APP_API ECS_ENTITY_DECLARE(ModRunning);
APP_API ECS_ENTITY_DECLARE(ModTerminated);

APP_API ECS_TAG_DECLARE(ModFlags);
APP_API ECS_ENTITY_DECLARE(ModReloadable);
APP_API ECS_ENTITY_DECLARE(ModNewerOnDisk);

APP_API ECS_COMPONENT_DECLARE(ModOnDisk);
APP_API ECS_COMPONENT_DECLARE(ModInRAM);

void ModsComponentsCoreLifecycleImport(ecs_world_t *world) {
  // https://www.flecs.dev/flecs/md_docs_2EntitiesComponents.html#registration
  // See the "modules" example
  ECS_MODULE(world, ModsComponentsCoreLifecycle);

  ECS_TAG_DEFINE(world, ModState);
  // Register ModState as exclusive relationship. This ensures that an entity
  // can only belong to a single ModState.
  ecs_add_id(world, ModState, EcsExclusive);

  ECS_ENTITY_DEFINE(world, ModAvailable);
  ECS_ENTITY_DEFINE(world, ModIncompatible);
  ECS_ENTITY_DEFINE(world, ModLoadFailed);
  ECS_ENTITY_DEFINE(world, ModInitFailed);
  ECS_ENTITY_DEFINE(world, ModRunning);
  ECS_ENTITY_DEFINE(world, ModTerminated);

  // ModFlags is not an exclusive relationship.
  ECS_TAG_DEFINE(world, ModFlags);
  ECS_ENTITY_DEFINE(world, ModReloadable);
  ECS_ENTITY_DEFINE(world, ModNewerOnDisk);

        // Components
  ECS_COMPONENT_DEFINE(world, ModOnDisk);
  ECS_COMPONENT_DEFINE(world, ModInRAM);
}
