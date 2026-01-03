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

typedef enum app_alloc_count_contexts {
    APP_CONTEXT_STARTUP_SHUTDOWN,
    APP_CONTEXT_FIRST_FRAMES,
    APP_CONTEXT_RENDERING,
    // add here and update app_alloc_count_contexts_str[][] in alloc.c
    APP_CONTEXT_COUNT
} app_alloc_count_contexts_t;

void alloc_count_install_hooks(void);
void alloc_count_dump_counters(Uint32 loops, char *when);
void alloc_count_set_context(app_alloc_count_contexts_t contextid);

void * SDLCALL alloc_count_malloc(size_t size);
void * SDLCALL alloc_count_malloc_userptr(size_t size, void *userptr);
void * SDLCALL alloc_count_malloc_ecs(ecs_size_t size); 
void * SDLCALL alloc_count_calloc(size_t nmemb, size_t size);
void * SDLCALL alloc_count_calloc_ecs(ecs_size_t size);
void * SDLCALL alloc_count_realloc(void *mem, size_t size);
void * SDLCALL alloc_count_realloc_ecs(void *mem, ecs_size_t size); 
void   SDLCALL alloc_count_free(void *mem);
void   SDLCALL alloc_count_free_ecs(void *mem);
void   SDLCALL alloc_count_free_userptr(void *mem, void *userptr);
char * SDLCALL alloc_count_strdup_ecs(const char *s);
