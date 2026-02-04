// Compile implementation of some header-only libraries here
#define DMON_IMPL
#include "app.h" // No "utils.h", embeded in app.h

APP_API char * strdup_or_null(const char *s) {
    // SDL_strdup uses SDL_malloc, accounting already done
    return s?SDL_strdup(s):NULL;
}

// Placeholder for future l10n support
APP_API const char * _(const char *string_to_localize) {
    return string_to_localize;
}

APP_API uint64_t convert_ns_to_frameid(uint64_t ns, int32_t framerate_num, int32_t framerate_den) {
    return (ns * framerate_num) / (1000000000 * framerate_den);
}
APP_API uint64_t convert_frameid_to_ns(uint64_t frameid, int32_t framerate_num, int32_t framerate_den) {
    return (frameid * 1000000000 * framerate_den ) / framerate_num;
}

void dmon_error_to_sdl_log_adapter(const char *msg) {
    app_error("%016"PRIu64" DMON %s", SDL_GetTicksNS(), msg);
}

void dmon_debug_to_sdl_log_adapter(const char *msg) {
    app_debug("%016"PRIu64" DMON %s", SDL_GetTicksNS(), msg);
}

//typedef void(* ecs_os_api_log_t) (int32_t level, const char *file, int32_t line, const char *msg)
void flecs_to_sdl_log_adapter(int32_t level, const char *file, int32_t line, const char *msg) {
    /* The level should be interpreted as:
     * >0: Debug tracing. Only enabled in debug builds.
     *  0: Tracing. Enabled in debug/release builds.
     * -2: Warning. An issue occurred, but operation was successful.
     * -3: Error. An issue occurred, and operation was unsuccessful.
     * -4: Fatal. An issue occurred, and application must quit.
     */
    char indent[32];
    indent[0] = '\0';
    if (level >= 0) {
        if (ecs_os_api.log_indent_) {
            int i, indent_count = ecs_os_api.log_indent_;
            if (indent_count > 15) indent_count = 15;

            for (i = 0; i < indent_count; i ++) {
                indent[i * 2] = '|';
                indent[i * 2 + 1] = ' ';
            }

            if (ecs_os_api.log_indent_ != indent_count) {
                indent[i * 2 - 2] = '+';
            }

            indent[i * 2] = '\0';
        }
    }
#define FLECS_LOG_VAARGS "%016"PRIu64" FLECS %s%s", SDL_GetTicksNS(), indent, msg
    switch (level) {
        case -4: app_critical(FLECS_LOG_VAARGS); break;
        case -3: app_error(FLECS_LOG_VAARGS); break;
        case -2: app_warn(FLECS_LOG_VAARGS); break;
        case  0: app_verbose(FLECS_LOG_VAARGS); break;
        default: app_trace(FLECS_LOG_VAARGS); break;
    }
#undef FLECS_LOG_VAARGS
}

void push_filesystem_event_to_sdl_queue(dmon_watch_id watch_id, dmon_action action,
        const char* rootdir, const char* filepath, const char* oldfilepath, void* user) {
    AppDmonEvent *devent = SDL_calloc(1, sizeof(AppDmonEvent));
    if (!devent) {
        // TODO decide a global behavior about OOM
        return;
    }
    devent->watch_id = watch_id;
    devent->action = action;
    devent->rootdir = strdup_or_null(rootdir);
    devent->filepath = strdup_or_null(filepath);
    devent->oldfilepath = strdup_or_null(oldfilepath);
    devent->user = user; //XXX this may lead to a double free() situation?

    SDL_Event user_event; // this is an union type, &(user_event) {...} syntax don't work
    SDL_zero(user_event);
    user_event.type = SDL_EVENT_USER;
    user_event.user.code = APP_USER_EVENT_FILESYSTEM;
    user_event.user.data1 = devent;
    user_event.user.data2 = NULL;
    SDL_PushEvent(&user_event);
}
