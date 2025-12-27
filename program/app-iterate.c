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
	
	// Make most of the work from ECS pipeline as it allow dynamic scheduling without recompiling this program
	ecs_progress(appstate->world, 0.0f);

	// Throttle in a way that try to get clean CFR video stream (Constant Frame Rate)
	// External video editing software needs CFR, streaming could be VFR but audio/video desync seems harder with VFR
	// We want to snap PTS (Presentation TimeStamps) for all video streams to a multiple of the framerate
	// Here we throttle to snap to SDL ticks so video code can just to have PTS = ticks - offset with offset = N/framerate.
	Uint64 step = (1000000000/appstate->main_framerate_num*appstate->main_framerate_den);
	Uint64 now = SDL_GetTicksNS();
	Uint64 sleep = step-(now%step);
	Uint64 next = now+sleep;
	SDL_DelayNS(sleep);
	app_debug("%016"PRIu64" SDL_AppIterate(): now:%"PRIu64", sleep:%"PRIu64", next:%"PRIu64,
				now, now, sleep, next);
	appstate->main_frame_start_ts = next;

	return appstate->app_result;
}
