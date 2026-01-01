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
SDL_AtomicInt alloc_count_per_context[APP_CONTEXT_COUNT][4];
SDL_AtomicInt *alloc_count_current_context = alloc_count_per_context[0];
SDL_malloc_func orig_malloc_func;
SDL_calloc_func orig_calloc_func;
SDL_realloc_func orig_realloc_func;
SDL_free_func orig_free_func;

const char app_alloc_count_contexts_str[APP_CONTEXT_COUNT][20] = {
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
	os_api.strdup_  = alloc_count_strdup_ecs;
	// Should not be in alloc.c but it fails if done in a second ecs_os_set_api() call
	os_api.log_     = flecs_to_sdl_log_adapter;
	os_api.now_     = SDL_GetTicksNS; // Hopefully less confusing to use the same everywhere
	ecs_os_set_api(&os_api);
}

void alloc_count_dump_counters(Uint32 loops, char *when) {
	if (when) app_info("%016"PRIu64" heap allocation at %s (only SDL_*alloc/SDL_free calls)", SDL_GetTicksNS(), when);
	app_info("[%7"PRIu32" loops]  ctxt   malloc   calloc  realloc     free (+diff)", loops);
	for ( int contextid = 0; contextid<APP_CONTEXT_COUNT; contextid++ ) {
		int malloc_count  = SDL_GetAtomicInt(&alloc_count_per_context[contextid][0]);
		int calloc_count  = SDL_GetAtomicInt(&alloc_count_per_context[contextid][1]);
		int realloc_count = SDL_GetAtomicInt(&alloc_count_per_context[contextid][2]);
		int free_count    = SDL_GetAtomicInt(&alloc_count_per_context[contextid][3]);

		app_info("%21s %8d %8d %8d %8d (%+d)", app_alloc_count_contexts_str[contextid],
				malloc_count, calloc_count, realloc_count, free_count,
				malloc_count + calloc_count - free_count);
	}
}

void alloc_count_set_context(app_alloc_count_contexts_t contextid) {
	if ( contextid >= 0 && contextid < APP_CONTEXT_COUNT ) {
		SDL_SetAtomicPointer((void **)&alloc_count_current_context, alloc_count_per_context[contextid]);
	}
}

void * SDLCALL alloc_count_malloc(size_t size) {
	SDL_AtomicIncRef(alloc_count_current_context+0);
	return orig_malloc_func(size);
}

void * SDLCALL alloc_count_malloc_ecs(ecs_size_t size) {
	SDL_AtomicIncRef(alloc_count_current_context+0);
	return orig_malloc_func(size);
}

void * SDLCALL alloc_count_malloc_userptr(size_t size, void *userptr) {
	(void) userptr;
	SDL_AtomicIncRef(alloc_count_current_context+0);
	return orig_malloc_func(size);
}

void * SDLCALL alloc_count_calloc(size_t nmemb, size_t size) {
	SDL_AtomicIncRef(alloc_count_current_context+1);
	return orig_calloc_func(nmemb, size);
}

void * SDLCALL alloc_count_calloc_ecs(ecs_size_t size) {
	SDL_AtomicIncRef(alloc_count_current_context+1);
	return orig_calloc_func(1, size);
}

void * SDLCALL alloc_count_realloc(void *mem, size_t size) {
	SDL_AtomicIncRef(alloc_count_current_context+2);
	return orig_realloc_func(mem, size);
}

void * SDLCALL alloc_count_realloc_ecs(void *mem, ecs_size_t size) {
	SDL_AtomicIncRef(alloc_count_current_context+2);
	return orig_realloc_func(mem, size);
}

void   SDLCALL alloc_count_free(void *mem) {
	SDL_AtomicIncRef(alloc_count_current_context+3);
	orig_free_func(mem);
}

void   SDLCALL alloc_count_free_ecs(void *mem) {
	SDL_AtomicIncRef(alloc_count_current_context+3);
	orig_free_func(mem);
}

void   SDLCALL alloc_count_free_userptr(void *mem, void *userptr) {
	(void) userptr;
	SDL_AtomicIncRef(alloc_count_current_context+3);
	orig_free_func(mem);
}

char * SDLCALL alloc_count_strdup_ecs(const char *s) {
	// SDL_strdup uses SDL_malloc, accounting already done
	return s?SDL_strdup(s):NULL;
	// return SDL_strdup(s); // WILL CRASH in ecs_init()
}
