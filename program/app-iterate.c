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
#include "ui.h"

SDL_AppResult SDL_AppIterate(void *_appstate) {
	appstate_t *appstate = (appstate_t *) _appstate;
	SDL_LogPriority logpriority_earlyskip = appstate->logpriority_earlyskip;
	
	SDL_AppResult then = SDL_APP_CONTINUE;

	ecs_progress(appstate->world, appstate->main_delta_time);

	// Throttle in a way that try to get clean CFR video stream (Constant Frame Rate)
	// External video editing software needs CFR, streaming could be VFR but audio/video desync seems harder with VFR
	// We want to snap PTS (Presentation TimeStamps) for all video streams to a multiple of the framerate
	// Here we throttle to snap to SDL ticks so video code can just to have PTS = ticks - offset with offset = N/framerate.
	Sint32 fr_num = appstate->main_framerate_num;
	Sint32 fr_den = appstate->main_framerate_den;
	Sint32 prev_frameid = appstate->main_frameid;
	Sint32 frameid = prev_frameid;
	Uint64 origin = appstate->main_frame_ticks_ns;
	Uint64 max_delay = 1e9/6; // Delay for a minimum of 6 FPS
	Uint64 now, next;
	// Send current frame to encoder(s), duplicate if we are in late, but stop if we go beyond max_delay to not freeze the UI too much
	do {
		//TODO send video frame now
		frameid++;
		next = frameid*1e9/fr_num*fr_den;
		now = SDL_GetTicksNS();
	} while (next < now || ((now-origin) > max_delay));

	if ( frameid - prev_frameid  > 1 ) {
		Sint32 skipped = frameid-prev_frameid-1;
		//TODO replace by a perf counter
		app_debug("%016"PRIu64" SDL_AppIterate(%"PRIi32"): skipped %"PRIi32" frame(s) (origin:%"PRIu64", next:%"PRIu64")",
				now, frameid, skipped, origin, next);
	}
	if ( next-now >= 1e9 ) {
		// Never hang for more than 1 sec, something is bad elsewhere
		app_warn("%016"PRIu64" SDL_AppIterate(%"PRIi32"): bad timestamps (next==%"PRIu64")", now, frameid, next);
		SDL_DelayNS(1e9/fr_num*fr_den); // delay by 1/FPS as safeguard guess with no valid time references
	} else {
		SDL_DelayNS(next-now);
		//app_trace("%016"PRIu64" SDL_AppIterate(%"PRIi32"): sleeping %"PRIu64" ns (origin:%"PRIu64", next:%"PRIu64")",
		//		now, frameid, next-now, origin, next);
	}

	appstate->main_frameid = frameid;
	appstate->main_frame_ticks_ns = next;
	appstate->main_delta_time = (next-origin)/1000000000.f;
	//app_trace("%016"PRIu64" SDL_AppIterate(%"PRIi32"): main_delta_time: %f", now, frameid, appstate->main_delta_time);

	return then;
}
