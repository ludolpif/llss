#
# Cross Platform Makefile
# Compatible with MSYS2/MINGW, Ubuntu 14.04.1 and Mac OS X
#
.PHONY: all clean app mods third-party run run-app-trace run-all-trace

all: app mods

# ordering rule to circumvent src and mods $(MAKE) call third-party $(MAKE) themselves and concurrently
app mods: third-party

app:
	$(MAKE) -C src

mods:
	$(MAKE) -C mods

third-party:
	$(MAKE) -C third-party

clean:
	$(MAKE) -C src clean
	$(MAKE) -C mods clean
	$(MAKE) -C third-party clean

run: all
	SDL_LOGGING="app=info,assert=warn,test=verbose,*=error" ./app

run-app-trace: all
	SDL_LOGGING="app=trace,assert=warn,test=verbose,*=error" ./app 2>&1 | dev/pretty-log.pl

run-all-trace: all
	SDL_LOGGING="*=trace" ./app 2>&1 | dev/pretty-log.pl
