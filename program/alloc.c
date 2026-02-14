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
#include "alloc.h"

// Those are global variables. All the appstate except this is on heap.
SDL_AtomicInt alloc_count_per_context[APP_CONTEXT_MAX][4];
SDL_AtomicInt *alloc_count_current_context = alloc_count_per_context[APP_CONTEXT_INVALID];
SDL_malloc_func orig_malloc_func;
SDL_calloc_func orig_calloc_func;
SDL_realloc_func orig_realloc_func;
SDL_free_func orig_free_func;

const char app_alloc_count_contexts_str[APP_CONTEXT_MAX][20] = {
    "INVALID",
    "STARTUP_SHUTDOWN",
    "FIRST_FRAMES",
    "RENDERING"
};

void alloc_count_install_hooks(void) {
    SDL_GetOriginalMemoryFunctions(&orig_malloc_func, &orig_calloc_func, &orig_realloc_func, &orig_free_func);
    SDL_SetMemoryFunctions(alloc_count_malloc, alloc_count_calloc, alloc_count_realloc, alloc_count_free);

    ImGui_SetAllocatorFunctions(alloc_count_malloc_userptr, alloc_count_free_userptr, NULL);

    ecs_os_set_api_defaults();
    ecs_os_api_t os_api = ecs_os_get_api();
    os_api.malloc_  = alloc_count_malloc_ecs;
    os_api.free_    = alloc_count_free_ecs;
    os_api.realloc_ = alloc_count_realloc_ecs;
    os_api.calloc_  = alloc_count_calloc_ecs;
    os_api.strdup_  = strdup_or_null; // using directly SDL_strdup will CRASH in ecs_init()
    // Should not be in alloc.c but it fails if done in a second ecs_os_set_api() call
    os_api.log_     = flecs_to_sdl_log_adapter;
    os_api.now_     = SDL_GetTicksNS; // Hopefully less confusing to use the same everywhere
    ecs_os_set_api(&os_api);
}

void alloc_count_dump_counters(Uint32 loops, char *when, int32_t contextid) {
    if (when) {
        app_info("%016"PRIu64" heap allocation at %s (only SDL_*alloc/SDL_free calls)",
                SDL_GetTicksNS(), when);
    }
    app_info("[%7"PRIu32" loops]  ctxt   malloc   calloc  realloc     free (+diff)", loops);
    int malloc_total = 0;
    int calloc_total = 0;
    int realloc_total = 0;
    int free_total = 0;
    int32_t min = APP_CONTEXT_INVALID+1, max = APP_CONTEXT_MAX;
    bool total = true;
    if ( contextid >= min && contextid < max ) {
        min = contextid; max = min+1;
        total = false;
    }
    for ( int i=min; i<max; i++ ) {
        int malloc_count  = SDL_GetAtomicInt(&alloc_count_per_context[i][0]);
        int calloc_count  = SDL_GetAtomicInt(&alloc_count_per_context[i][1]);
        int realloc_count = SDL_GetAtomicInt(&alloc_count_per_context[i][2]);
        int free_count    = SDL_GetAtomicInt(&alloc_count_per_context[i][3]);

        app_info("%21s %8d %8d %8d %8d (%+d)", app_alloc_count_contexts_str[i],
                malloc_count, calloc_count, realloc_count, free_count,
                malloc_count + calloc_count - free_count);

        malloc_total += malloc_count;
        calloc_total += calloc_count;
        realloc_total += realloc_count;
        free_total += free_count;
    }
    if (total) {
        app_info("%21s %8d %8d %8d %8d (%+d)", "TOTAL",
                malloc_total, calloc_total, realloc_total, free_total,
                malloc_total + calloc_total - free_total);
    }
}

void alloc_count_set_context(app_alloc_count_contexts_t contextid) {
    if ( contextid >= 0 && contextid < APP_CONTEXT_MAX ) {
        SDL_SetAtomicPointer((void **)&alloc_count_current_context, alloc_count_per_context[contextid]);
    }
}

void * SDLCALL alloc_count_malloc(size_t size) {
    if (size) {
        SDL_AtomicIncRef(alloc_count_current_context+0);
    } else {
        app_debug("%016"PRIu64" alloc_count_malloc(0) called", SDL_GetTicksNS());
    }
    return orig_malloc_func(size);
}

void * SDLCALL alloc_count_malloc_ecs(ecs_size_t size) {
    if (size) {
        SDL_AtomicIncRef(alloc_count_current_context+0);
    } else {
        app_debug("%016"PRIu64" alloc_count_malloc_ecs(0) called", SDL_GetTicksNS());
    }
    return orig_malloc_func(size);
}

void * SDLCALL alloc_count_malloc_userptr(size_t size, void *userptr) {
    (void) userptr;
    if (size) {
        SDL_AtomicIncRef(alloc_count_current_context+0);
    } else {
        app_debug("%016"PRIu64" alloc_count_malloc_userptr(0) called", SDL_GetTicksNS());
    }
    return orig_malloc_func(size);
}

void * SDLCALL alloc_count_calloc(size_t nmemb, size_t size) {
    if (nmemb && size) {
        SDL_AtomicIncRef(alloc_count_current_context+1);
    } else if (!nmemb) {
        app_debug("%016"PRIu64" alloc_count_calloc(0, size) called", SDL_GetTicksNS());
    } else if (!size) {
        app_debug("%016"PRIu64" alloc_count_calloc(nmemb, 0) called", SDL_GetTicksNS());
    }
    return orig_calloc_func(nmemb, size);
}

void * SDLCALL alloc_count_calloc_ecs(ecs_size_t size) {
    if (size) {
        SDL_AtomicIncRef(alloc_count_current_context+1);
    } else {
        app_debug("%016"PRIu64" alloc_count_calloc_ecs(0) called", SDL_GetTicksNS());
    }
    return orig_calloc_func(1, size);
}

void * SDLCALL alloc_count_realloc(void *mem, size_t size) {
    void *new = orig_realloc_func(mem, size);
    if (mem && size) {
        SDL_AtomicIncRef(alloc_count_current_context+2);
    } else if (!size) {
        app_debug("%016"PRIu64" alloc_count_realloc(mem, 0) called", SDL_GetTicksNS());
        SDL_AtomicIncRef(alloc_count_current_context+3);
    } else if (!mem) {
        // no log because SDL3 make some realloc(NULL, size) calls
        //app_debug("%016"PRIu64" alloc_count_realloc(NULL, size) called", SDL_GetTicksNS());
        SDL_AtomicIncRef(alloc_count_current_context+0);
    }
    return new;
}

void * SDLCALL alloc_count_realloc_ecs(void *mem, ecs_size_t size) {
    void *new = orig_realloc_func(mem, size);
    if (mem && size) {
        SDL_AtomicIncRef(alloc_count_current_context+2);
    } else if (!size) {
        app_debug("%016"PRIu64" alloc_count_realloc_ecs(mem, 0) called", SDL_GetTicksNS());
        SDL_AtomicIncRef(alloc_count_current_context+3);
    } else if (!mem) {
        // no log because flecs make many realloc(NULL, size) calls
        //app_debug("%016"PRIu64" alloc_count_realloc_ecs(NULL, size) called", SDL_GetTicksNS());
        SDL_AtomicIncRef(alloc_count_current_context+0);
    }
    return new;
}

void   SDLCALL alloc_count_free(void *mem) {
    if (mem) {
        SDL_AtomicIncRef(alloc_count_current_context+3);
        orig_free_func(mem);
    } else {
        app_debug("%016"PRIu64" alloc_count_free(NULL) called", SDL_GetTicksNS());
    }
}

void   SDLCALL alloc_count_free_ecs(void *mem) {
    if (mem) {
        SDL_AtomicIncRef(alloc_count_current_context+3);
        orig_free_func(mem);
    }
    // no else { app_debug(...); } because flecs make many free(NULL) calls
}

void   SDLCALL alloc_count_free_userptr(void *mem, void *userptr) {
    (void) userptr;
    if (mem) {
        SDL_AtomicIncRef(alloc_count_current_context+3);
        orig_free_func(mem);
    } else {
        app_debug("%016"PRIu64" alloc_count_free_ecs_userptr(NULL) called", SDL_GetTicksNS());
    }
}
