#
# Cross Platform Makefile
# Compatible with MSYS2/MINGW, Ubuntu 14.04.1 and Mac OS X
#
.PHONY: all clean llss mods run run-app-trace run-all-trace

all: llss mods

llss:
	$(MAKE) -C src

mods:
	$(MAKE) -C mods

clean:
	$(MAKE) -C src clean
	$(MAKE) -C mods clean
	$(MAKE) -C third-party/static/ecs clean
	$(MAKE) -C third-party/static/ui clean

run: all
	SDL_LOGGING="app=info,assert=warn,test=verbose,*=error" ./llss

run-app-trace: all
	SDL_LOGGING="app=trace,assert=warn,test=verbose,*=error" ./llss 2>&1 | dev/pretty-log.pl

run-all-trace: all
	SDL_LOGGING="*=trace" ./llss 2>&1 | dev/pretty-log.pl
