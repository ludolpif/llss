#pragma once
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

// FLECS Reflection system boilerplate
#undef ECS_META_IMPL
#ifndef APP_COMPONENTS_CORE_IMPL
#define ECS_META_IMPL EXTERN // Ensure meta symbols are only defined once
#endif

APP_API void AppComponentsCoreImport(ecs_world_t *world);

// Phases for pipelines
extern APP_API ecs_entity_t RenderingPreImGui, RenderingOnImGui, RenderingPostImGui;

// Elements for async IO state
extern APP_API ECS_TAG_DECLARE(IOState);
extern APP_API ECS_ENTITY_DECLARE(IOComplete);
extern APP_API ECS_ENTITY_DECLARE(IOFailure);
extern APP_API ECS_ENTITY_DECLARE(IOCanceled);

// Components
APP_API ECS_STRUCT(AppVersion, {
    int32_t running_app_version;
    int32_t build_dep_version_compiled_against;
});

typedef struct {
    // TODO this approach don't cover per-thread arena
    SDL_malloc_func sdl_malloc_func;
    SDL_calloc_func sdl_calloc_func;
    SDL_realloc_func sdl_realloc_func;
    SDL_free_func sdl_free_func;
    ImGuiMemAllocFunc imgui_malloc_func;
    ImGuiMemFreeFunc imgui_free_func;
    void* imgui_allocator_functions_user_data;
    // flecs alloc_funcs can always be retreived with ecs_os_get_api()
} AppMemoryFuncs;
extern APP_API ECS_COMPONENT_DECLARE(AppMemoryFuncs);

typedef struct {
    SDL_Window *main_window;
    SDL_GPUDevice *gpu_device;
    SDL_AsyncIOQueue *sdl_io_queue;
} AppSDLContext;
extern APP_API ECS_COMPONENT_DECLARE(AppSDLContext);

typedef struct {
    ImGuiContext* imgui_context;
    ImGuiIO *imgui_io;
} AppImGuiContext;
extern APP_API ECS_COMPONENT_DECLARE(AppImGuiContext);

APP_API ECS_STRUCT(AppMainTimingContext, {
    uint32_t app_iterate_count;
    uint32_t total_skipped;
    int32_t main_framerate_num;  // AVRational framerate numerator
    int32_t main_framerate_den;  // AVRational framerate denominator
    uint64_t main_frame_start_ns; // In SDL_GetTicksNS() format, snapped to multiple of main_framerate
    uint64_t main_frameid; // Unique identifier for current frame, garanted monotonic until main_framerate changes
});

typedef SDL_AsyncIOOutcome AsyncIOOutcome;
extern APP_API ECS_COMPONENT_DECLARE(AsyncIOOutcome);

typedef struct {
    dmon_watch_id watch_id;
    dmon_action action;
    const char *rootdir;
    const char *filepath;
    const char *oldfilepath;
    void *user;
} AppDmonEvent;
extern APP_API ECS_COMPONENT_DECLARE(AppDmonEvent);

