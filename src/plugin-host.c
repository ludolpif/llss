#include "../internal/plugin-host.h"

SDL_SharedObject *plugin1;
unsigned (*avformat_version)();

bool plugin_host_init() {
	//TODO this is dummy code for first sanity checks
	plugin1 = SDL_LoadObject("/usr/lib/x86_64-linux-gnu/libavformat.so.61");
	if (!plugin1) return false;
	avformat_version = (unsigned (*)())SDL_LoadFunction(plugin1, "avformat_version");
	if (!avformat_version) return false;
	unsigned avv = avformat_version();
	app_warn("avformat_version returns %u.%u.%u", avv>>16, (avv&0xff00)>>8, (avv&0xff));
	return true;
}
