#include "app.h" // No "utils.h", embeded in app.h

APP_API Uint64 convert_ns_to_frameid(Uint64 ns, Sint32 framerate_num, Sint32 framerate_den) {
	return (ns * framerate_num) / (1000000000 * framerate_den);
}
APP_API Uint64 convert_frameid_to_ns(Uint64 frameid, Sint32 framerate_num, Sint32 framerate_den) {
	return (frameid * 1000000000 * framerate_den ) / framerate_num;
}
