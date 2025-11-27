#
# Cross Platform Makefile
# Compatible with MSYS2/MINGW, Ubuntu 14.04.1 and Mac OS X
#
.PHONY: all clean clean-recursive run


# TODO https://media.bernat.ch/files/debian-debug-packages.pdf

all: src/llss

src/llss:
	$(MAKE) -C src

clean:
	$(MAKE) -C src clean

clean-recursive:
	$(MAKE) -C src clean
	$(MAKE) -C third-party/static/ecs clean
	$(MAKE) -C third-party/static/ui clean

run: src/llss
	SDL_LOGGING="app=info,assert=warn,test=verbose,*=error" src/llss

run-app-trace: src/llss
	SDL_LOGGING="app=trace,assert=warn,test=verbose,*=error" src/llss

run-all-trace: llss
	SDL_LOGGING="*=trace" src/llss

