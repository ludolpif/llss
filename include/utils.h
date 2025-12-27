#pragma once
#include <SDL3/SDL_stdinc.h>

Uint64 convert_ns_to_frameid(Uint64 ns, Sint32 framerate_num, Sint32 framerate_den);
Uint64 convert_frameid_to_ns(Uint64 frameid, Sint32 framerate_num, Sint32 framerate_den);

// For static initializers, we cannot use the function, so use a difficult to use macro (implicit casts can destroy data)
#define CONVERT_FRAMEID_TO_NS(frameid, framerate_num, framerate_den) \
	( ( ( (Uint64)frameid) * 1000000000 * framerate_den ) / framerate_num )

