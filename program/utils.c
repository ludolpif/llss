#include "app.h" // No "utils.h", embeded in app.h

APP_API uint64_t convert_ns_to_frameid(uint64_t ns, int32_t framerate_num, int32_t framerate_den) {
	return (ns * framerate_num) / (1000000000 * framerate_den);
}
APP_API uint64_t convert_frameid_to_ns(uint64_t frameid, int32_t framerate_num, int32_t framerate_den) {
	return (frameid * 1000000000 * framerate_den ) / framerate_num;
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
