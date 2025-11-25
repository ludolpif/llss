#pragma once
#include "app.h"

typedef enum app_alloc_count_contexts {
	APP_CONTEXT_STARTUP,
	APP_CONTEXT_FIRST_FRAMES,
	APP_CONTEXT_RENDERING,
	APP_CONTEXT_SHUTDOWN,
	// add here...
	APP_CONTEXT_COUNT
} app_alloc_count_contexts_t;

void alloc_count_install_hooks();
void alloc_count_dump_counters();
void alloc_count_set_context(app_alloc_count_contexts_t contextid);

void * SDLCALL alloc_count_malloc(size_t size);
void * SDLCALL alloc_count_calloc(size_t nmemb, size_t size);
void * SDLCALL alloc_count_realloc(void *mem, size_t size);
void   SDLCALL alloc_count_free(void *mem);
