#
# Cross Platform Makefile
# Compatible with MSYS2/MINGW, Ubuntu 14.04.1 and Mac OS X
#
.PHONY: all clean clean-recursive run

##---------------------------------------------------------------------
## BUILD RULES
##---------------------------------------------------------------------

all:
	git submodule update --init --recursive third-party/static/ui/imgui
	$(MAKE) -C src

clean:
	$(MAKE) -C src clean

clean-recursive:
	$(MAKE) -C src clean
	$(MAKE) -C third-party/static/ecs clean
	$(MAKE) -C third-party/static/ui clean

run:
	$(MAKE) -C src run
