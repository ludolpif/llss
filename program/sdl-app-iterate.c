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
#include "dcimgui_impl_sdl3.h"
#include "dcimgui_impl_sdlgpu3.h"
#include "alloc.h"

SDL_AppResult SDL_AppIterate(void *appstate) {
    static int32_t prev_fr_num = 60;
    static int32_t prev_fr_den = 1;
    static uint64_t sleep_max = CONVERT_FRAMEID_TO_NS(1, 60, 1);

    ecs_world_t *world = (ecs_world_t *)appstate;

    // Make most of the work from ECS pipeline as it allow dynamic scheduling without recompiling this program
    ecs_progress(world, 0.0f);

    // Throttle in a way that helps to get clean CFR video stream (Constant Frame Rate)
    // External video editing software needs CFR, streaming could be VFR but audio/video sync seems harder with VFR
    // We want to snap PTS (Presentation TimeStamps) for all video streams to a multiple of the framerate
    // Here we throttle to snap to SDL ticks so video code can just to have PTS = ticks - offset with offset = N/framerate.
    AppMainTimingContext *app_main_timing_context = ecs_singleton_get_mut(world, AppMainTimingContext);
    int32_t fr_num = app_main_timing_context->main_framerate_num;
    int32_t fr_den = app_main_timing_context->main_framerate_den;

    uint64_t last_frameid = app_main_timing_context->main_frameid;
    uint64_t now_ns = SDL_GetTicksNS();
    uint64_t now_frameid = convert_ns_to_frameid(now_ns, fr_num, fr_den);
    uint64_t next_frameid = now_frameid + 1;
    uint64_t next_ns = convert_frameid_to_ns(next_frameid, fr_num, fr_den);

    uint64_t sleep_ns;
    uint32_t skipped;
    // If main_framerate just have changed we may have negative jumps, and it cannnot fit in uint64_t
    if ( prev_fr_num == fr_num && prev_fr_den == fr_den ) {
        sleep_ns = next_ns - now_ns;
        // If unsigned arithmetics goes wrong, never hang the program more than 1/FPS seconds
        if ( sleep_ns > sleep_max ) sleep_ns = sleep_max;
        // If main_framerate have changed 1 iteration ago, last_frameid may be greated than now_frameid
        skipped = (now_frameid > last_frameid)?(uint32_t)(now_frameid - last_frameid):0;
    } else {
        app_warn("%016"PRIu64" SDL_AppIterate(), framerate changed from %"PRIi32"/%"PRIi32" to %"PRIi32"/%"PRIi32,
                now_ns, prev_fr_num, prev_fr_den, fr_num, fr_den);
        sleep_max = convert_frameid_to_ns(1, fr_num, fr_den);
        sleep_ns = sleep_max;
        skipped = 0;
    }

#if DEBUG_TIMING
    app_trace("%016"PRIu64" SDL_AppIterate()"
            ", last_frameid:%09"PRIu64
            ", now_frameid:%09"PRIu64
            ", next_ns:%016"PRIu64
            ", sleep_ns:%09"PRIu64
            ", skipped:%"PRIu32
            ,now_ns, last_frameid, now_frameid, next_ns, sleep_ns, skipped);
#endif
    SDL_DelayNS(sleep_ns);

    prev_fr_num = fr_num;
    prev_fr_den = fr_den;
    app_main_timing_context->app_iterate_count++;
    app_main_timing_context->total_skipped += skipped;
    if ( app_main_timing_context->app_iterate_count == 5 ) alloc_count_set_context(APP_CONTEXT_RENDERING);
    app_main_timing_context->main_frameid = next_frameid;
    app_main_timing_context->main_frame_start_ns = next_ns;

    // TODO trigger QUIT here depending on an info from the ECS
    return SDL_APP_CONTINUE;
}
