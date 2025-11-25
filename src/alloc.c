#include "../internal/alloc.h"

// Those are global variables. All the appstate except this is on heap.
SDL_AtomicInt alloc_count_per_context[APP_CONTEXT_COUNT];
SDL_AtomicInt *alloc_count_current_context = alloc_count_per_context;
SDL_malloc_func orig_malloc_func;
SDL_calloc_func orig_calloc_func;
SDL_realloc_func orig_realloc_func;
SDL_free_func orig_free_func;


void alloc_count_install_hooks() {
	SDL_GetOriginalMemoryFunctions(&orig_malloc_func, &orig_calloc_func, &orig_realloc_func, &orig_free_func);
	SDL_SetMemoryFunctions(alloc_count_malloc, alloc_count_calloc, alloc_count_realloc, alloc_count_free);
}

void alloc_count_dump_counters() {
	bool skip_debug = false;
	app_trace("%016lu alloc_count_dump_counters()", SDL_GetTicksNS());
	for ( int contextid = 0; contextid<APP_CONTEXT_COUNT; contextid++ ) {
		int count = SDL_GetAtomicInt(&alloc_count_per_context[contextid]);
		app_trace("                 alloc_count_per_context[%d]==%d", contextid, count);
	}
}

void alloc_count_set_context(app_alloc_count_contexts_t contextid) {
	if ( contextid >= 0 && contextid < APP_CONTEXT_COUNT ) {
		SDL_SetAtomicPointer((void **)&alloc_count_current_context, &alloc_count_per_context[contextid]);
	}
}

void * SDLCALL alloc_count_malloc(size_t size) {
	SDL_AtomicIncRef(alloc_count_current_context);
	return orig_malloc_func(size);
}

void * SDLCALL alloc_count_calloc(size_t nmemb, size_t size) {
	SDL_AtomicIncRef(alloc_count_current_context);
	return orig_calloc_func(nmemb, size);
}

void * SDLCALL alloc_count_realloc(void *mem, size_t size) {
	//FIXME make this call useful or skip it
	return orig_realloc_func(mem, size);
}

void   SDLCALL alloc_count_free(void *mem) {
	SDL_AtomicDecRef(alloc_count_current_context);
	orig_free_func(mem);
}
