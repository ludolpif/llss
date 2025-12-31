#define DLL_EXPORT
#include "app.h" // No "ecs-mods-state.h", embeded in app.h

ECS_ENTITY_DECLARE(Available);
ECS_ENTITY_DECLARE(Incompatible);
ECS_ENTITY_DECLARE(Loaded);
ECS_ENTITY_DECLARE(LoadFailed);
ECS_ENTITY_DECLARE(InitFailed);
ECS_ENTITY_DECLARE(Running);
ECS_ENTITY_DECLARE(Terminated);

void ModsStateImport(ecs_world_t *world) {
	ECS_MODULE(world, ModsState);

	ECS_ENTITY_DEFINE(world, Available);
	ECS_ENTITY_DEFINE(world, Incompatible);
	ECS_ENTITY_DEFINE(world, LoadFailed);
	ECS_ENTITY_DEFINE(world, InitFailed);
	ECS_ENTITY_DEFINE(world, Running);
	ECS_ENTITY_DEFINE(world, Terminated);
}
