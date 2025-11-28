#pragma once
#include "app.h"

typedef enum app_alloc_count_contexts {
	APP_CONTEXT_STARTUP_SHUTDOWN,
	APP_CONTEXT_FIRST_FRAMES,
	APP_CONTEXT_RENDERING,
	// add here and update app_alloc_count_contexts_str[][] in alloc.c
	APP_CONTEXT_COUNT
} app_alloc_count_contexts_t;

void alloc_count_install_hooks();
void alloc_count_dump_counters();
void alloc_count_set_context(app_alloc_count_contexts_t contextid);

void * SDLCALL alloc_count_malloc(size_t size);
void * SDLCALL alloc_count_malloc_userptr(size_t size, void *userptr);
void * SDLCALL alloc_count_calloc(size_t nmemb, size_t size);
void * SDLCALL alloc_count_realloc(void *mem, size_t size);
void   SDLCALL alloc_count_free(void *mem);
void   SDLCALL alloc_count_free_userptr(void *mem, void *userptr);
