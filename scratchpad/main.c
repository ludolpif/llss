#include "app.h"
#include "alloc.h"
#include "scratch.h"

#define app_failure(...) do { \
    SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, __VA_ARGS__); \
    return SDL_APP_FAILURE; \
} while(SDL_NULL_WHILE_LOOP_CONDITION)

int ecs_worker_threads_count = 4;
SDL_LogPriority logpriority_earlyskip = SDL_LOG_PRIORITY_TRACE;
Uint32 APP_USER_EVENT_FILESYSTEM;

int main(int argc, char **argv) {
    // Configure memory functions before the first dynamic allocation
    alloc_count_install_hooks();

    // Configure logging
#ifdef _DEBUG
    // Set default logging as SDL3 default logging, but with app=trace instead of app=info
    SDL_SetHint(SDL_HINT_LOGGING, "app=trace,assert=warn,test=verbose,*=error");
#endif
    logpriority_earlyskip = SDL_GetLogPriority(SDL_LOG_CATEGORY_APPLICATION);
    SDL_SetLogPriorityPrefix(SDL_LOG_PRIORITY_TRACE, ECS_GREY   "TRACE " ECS_NORMAL);
    SDL_SetLogPriorityPrefix(SDL_LOG_PRIORITY_VERBOSE, ECS_GREY "VERB  " ECS_NORMAL);
    SDL_SetLogPriorityPrefix(SDL_LOG_PRIORITY_DEBUG, ECS_GREY   "DEBUG " ECS_NORMAL);
    SDL_SetLogPriorityPrefix(SDL_LOG_PRIORITY_INFO, ECS_MAGENTA "INFO  " ECS_NORMAL);
    SDL_SetLogPriorityPrefix(SDL_LOG_PRIORITY_WARN, ECS_YELLOW  "WARN  " ECS_NORMAL);
    SDL_SetLogPriorityPrefix(SDL_LOG_PRIORITY_ERROR, ECS_RED    "ERROR " ECS_NORMAL);
    SDL_SetLogPriorityPrefix(SDL_LOG_PRIORITY_CRITICAL, ECS_RED "CRIT  " ECS_NORMAL);

    /*
     * Early log message to help troubleshoot application init and allow human readable
     *  timestamps later conversion. Note that SDL_Ticks should be a CLOCK_MONOTONIC source,
     *  but some platforms may not provide it.
     *  SDL_Time is real time, subject to system clock adjustment
     */
    SDL_Time tick0_wallclock = 0;
    (void) SDL_GetCurrentTime(&tick0_wallclock);

    Uint64 tick = SDL_GetTicksNS();
    tick0_wallclock -= tick;
    app_warn("Starting %s %s with SDL %s logpriority:%d, tick0_wallclock:%"PRId64,
            APP_METADATA_NAME_STRING, APP_VERSION_STR, SDL_GetRevision(),
            logpriority_earlyskip, tick0_wallclock);


    APP_USER_EVENT_FILESYSTEM = SDL_RegisterEvents(1);
    if (!APP_USER_EVENT_FILESYSTEM)
        app_failure("SDL_RegisterEvents(1) for APP_USER_EVENT_FILESYSTEM: %s", SDL_GetError());

    ecs_world_t *world = ecs_init_w_args(argc, argv);
    // ecs_set_threads(world, ecs_worker_threads_count); // this make randomly huge execution time with valgrind
    ECS_IMPORT(world, FlecsStats); // Optional, enhance for https://www.flecs.dev/explorer
    ecs_log_set_level(0); // Increase verbosity level
    ecs_singleton_set(world, EcsRest, {0}); // Creates REST server on default port (27750)

    init_hook(world);

    bool cont = true;
    while ( cont ) {
        cont = ecs_progress(world, 0.0f) && tick_hook(world);
        SDL_Delay(10);
    }

    fini_hook(world);
    ecs_log_set_level(-1);
    ecs_fini(world);
    return 0;
}
