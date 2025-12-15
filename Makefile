#
# Cross Platform Makefile
# Compatible with MSYS2/MINGW, Ubuntu 14.04.1 and Mac OS X
#
.PHONY: all clean app mods lib run run-app-trace run-all-trace

# default value for command-line arguments (like make BUILD_TYPE=RelWithDebInfo)
BUILD_TYPE ?= Debug

all: app mods

# ordering rule. if dependencies are expressed in subdirs Makefile then calling here a 
# "make -j" can call $(MAKE) -C lib multiple times, concurrently and it will fail
app mods: lib

app:
	$(MAKE) -C src BUILD_TYPE=$(BUILD_TYPE)

mods:
	$(MAKE) -C mods BUILD_TYPE=$(BUILD_TYPE)

lib:
	$(MAKE) -C lib BUILD_TYPE=$(BUILD_TYPE)

clean:
	$(MAKE) -C src clean BUILD_TYPE=$(BUILD_TYPE)
	$(MAKE) -C mods clean BUILD_TYPE=$(BUILD_TYPE)
	$(MAKE) -C lib clean BUILD_TYPE=$(BUILD_TYPE)

run: all
	SDL_LOGGING="app=info,assert=warn,test=verbose,*=error" ./app

run-app-trace: all
	SDL_LOGGING="app=trace,assert=warn,test=verbose,*=error" ./app 2>&1 | dev/pretty-log.pl

run-all-trace: all
	SDL_LOGGING="*=trace" ./app 2>&1 | dev/pretty-log.pl
