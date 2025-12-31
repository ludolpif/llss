#include "app.h" // No "ecs-mods-state.h", embeded in app.h

APP_API ECS_ENTITY_DECLARE(ModAvailable);
APP_API ECS_ENTITY_DECLARE(ModIncompatible);
APP_API ECS_ENTITY_DECLARE(ModLoaded);
APP_API ECS_ENTITY_DECLARE(ModLoadFailed);
APP_API ECS_ENTITY_DECLARE(ModInitFailed);
APP_API ECS_ENTITY_DECLARE(ModRunning);
APP_API ECS_ENTITY_DECLARE(ModTerminated);

void ModsStateImport(ecs_world_t *world) {
	ECS_MODULE(world, ModsState);

	ECS_ENTITY_DEFINE(world, ModAvailable);
	ECS_ENTITY_DEFINE(world, ModIncompatible);
	ECS_ENTITY_DEFINE(world, ModLoadFailed);
	ECS_ENTITY_DEFINE(world, ModInitFailed);
	ECS_ENTITY_DEFINE(world, ModRunning);
	ECS_ENTITY_DEFINE(world, ModTerminated);
}
