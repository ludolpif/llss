#
# Cross Platform Makefile
# Compatible with MSYS2/MINGW, Ubuntu 14.04.1 and Mac OS X
#
.PHONY: all clean clean-recursive llss run


# TODO https://media.bernat.ch/files/debian-debug-packages.pdf

all: llss mods/llss-mod-template/llss-mod-template.so

llss:
	$(MAKE) -C src

mods/llss-mod-template/llss-mod-template.so:
	$(MAKE) -C mods/llss-mod-template

clean:
	$(MAKE) -C src clean

clean-recursive:
	$(MAKE) -C src clean
	$(MAKE) -C mods/llss-mod-template clean
	$(MAKE) -C third-party/static/ecs clean
	$(MAKE) -C third-party/static/ui clean

run: llss
	SDL_LOGGING="app=info,assert=warn,test=verbose,*=error" ./llss

run-app-trace: llss
	SDL_LOGGING="app=trace,assert=warn,test=verbose,*=error" ./llss

run-all-trace: llss
	SDL_LOGGING="*=trace" ./llss

