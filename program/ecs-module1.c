#include "app.h"
// No ecs-module1.h, embeded in app.h

ECS_COMPONENT_DECLARE(AppIterateGlobalFrameCounters);
ECS_COMPONENT_DECLARE(Position);

void LogDeltaTime(ecs_iter_t *it) {
	SDL_LogPriority logpriority_earlyskip = SDL_LOG_PRIORITY_TRACE; // TODO get it from ECS, ditto for frameid
	// Print delta_time. The same value is passed to all systems.
	app_trace("%016"PRIu64" ecs_iter_t it->delta_time: %f, it->delta_system_time: %f\n",
			SDL_GetTicksNS(), (double)it->delta_time, (double)it->delta_system_time);
}

void Module1Import(ecs_world_t *world) {
	// https://www.flecs.dev/flecs/md_docs_2EntitiesComponents.html#registration
	// See the "modules" example
	ECS_MODULE(world, Module1);

	ECS_COMPONENT_DEFINE(world, AppIterateGlobalFrameCounters);
	ECS_COMPONENT_DEFINE(world, Position);

	// https://www.flecs.dev/flecs/md_docs_2EntitiesComponents.html#singletons
 	ecs_singleton_set(world, AppIterateGlobalFrameCounters, {0});

	ECS_SYSTEM(world, LogDeltaTime, EcsOnUpdate, 0);
}

