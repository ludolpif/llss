/*
 * This file is part of LLSS.
 *
 * LLSS is free software: you can redistribute it and/or modify it under the terms of the
 * Affero GNU General Public License as published by the Free Software Foundation,
 * either version 3 of the License, or (at your option) any later version.
 *
 * LLSS is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with LLSS.
 * If not, see <https://www.gnu.org/licenses/>. See LICENSE file at root of this git repo.
 *
 * Copyright 2025 ludolpif <ludolpif@gmail.com>
 */
#include "app.h"
#include "app-internal.h"
#include "dcimgui_impl_sdl3.h"
#include "dcimgui_impl_sdlgpu3.h"
#include "alloc.h"
#include "utils.h"

SDL_AppResult SDL_AppIterate(void *_appstate) {
	static Sint32 prev_framerate_num = 60;
	static Sint32 prev_framerate_den = 1;
	static Uint64 sleep_max = CONVERT_FRAMEID_TO_NS(1, 60, 1);

	appstate_t *appstate = (appstate_t *) _appstate;
	SDL_LogPriority logpriority_earlyskip = appstate->logpriority_earlyskip;
	
	// Make most of the work from ECS pipeline as it allow dynamic scheduling without recompiling this program
	ecs_progress(appstate->world, 0.0f);

	// Throttle in a way that try to get clean CFR video stream (Constant Frame Rate)
	// External video editing software needs CFR, streaming could be VFR but audio/video desync seems harder with VFR
	// We want to snap PTS (Presentation TimeStamps) for all video streams to a multiple of the framerate
	// Here we throttle to snap to SDL ticks so video code can just to have PTS = ticks - offset with offset = N/framerate.
	Sint32 framerate_num = appstate->main_framerate_num;
	Sint32 framerate_den = appstate->main_framerate_den;

	Uint64 last_frameid = appstate->main_frameid;
	Uint64 now_ns = SDL_GetTicksNS();
	Uint64 now_frameid = convert_ns_to_frameid(now_ns, framerate_num, framerate_den);
	Uint64 next_frameid = now_frameid + 1;
	Uint64 next_ns = convert_frameid_to_ns(next_frameid, framerate_num, framerate_den);

	Uint64 sleep_ns;
	Uint64 skipped;
	// If main_framerate just have changed we may have negative jumps, and it cannnot fit in Uint64
	if ( prev_framerate_num == framerate_num && prev_framerate_den == framerate_den ) {
		sleep_ns = next_ns - now_ns;
		// If unsigned arithmetics goes wrong, never hang the program more than 1/FPS seconds
		if ( sleep_ns > sleep_max ) sleep_ns = sleep_max;
		// If main_framerate have changed 1 iteration ago, last_frameid may be greated than now_frameid
		skipped = (now_frameid > last_frameid)?(now_frameid - last_frameid):0;
	} else {
		app_warn("%016"PRIu64" SDL_AppIterate(), framerate changed from %"PRIi32"/%"PRIi32" to %"PRIi32"/%"PRIi32,
				now_ns, prev_framerate_num, prev_framerate_den, framerate_num, framerate_den);
		sleep_max = convert_frameid_to_ns(1, framerate_num, framerate_den);
		sleep_ns = sleep_max;
		skipped = 0;
	}

	app_trace("%016"PRIu64" SDL_AppIterate()"
			", last_frameid:%09"PRIu64
			", now_frameid:%09"PRIu64
			", next_ns:%016"PRIu64
			", sleep_ns:%09"PRIu64
			", skipped:%"PRIu64
			,now_ns, last_frameid, now_frameid, next_ns, sleep_ns, skipped);
	SDL_DelayNS(sleep_ns);

	prev_framerate_num = framerate_num;
	prev_framerate_den = framerate_den;
	appstate->app_iterate_count++;
	if ( appstate->app_iterate_count == 5 ) alloc_count_set_context(APP_CONTEXT_RENDERING);
	appstate->main_frameid = next_frameid;
	appstate->main_frame_start_ns = next_ns;

	return appstate->app_result;
}
