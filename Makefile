#
# Cross Platform Makefile
# Compatible with MSYS2/MINGW, Ubuntu 14.04.1 and Mac OS X
#
.PHONY: all clean app mods lib run run-app-trace run-all-trace

all: app mods

# ordering rule. if dependencies are expressed subdirs Makefile
# then calling here make -j can call $(MAKE) -C lib multiple times, concurrently
app mods: lib

app:
	$(MAKE) -C src

mods:
	$(MAKE) -C mods

lib:
	$(MAKE) -C lib

clean:
	$(MAKE) -C src clean
	$(MAKE) -C mods clean
	$(MAKE) -C lib clean

run: all
	SDL_LOGGING="app=info,assert=warn,test=verbose,*=error" ./app

run-app-trace: all
	SDL_LOGGING="app=trace,assert=warn,test=verbose,*=error" ./app 2>&1 | dev/pretty-log.pl

run-all-trace: all
	SDL_LOGGING="*=trace" ./app 2>&1 | dev/pretty-log.pl
