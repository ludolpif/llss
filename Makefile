#
# Cross Platform Makefile
# Compatible with MSYS2/MINGW, Ubuntu 14.04.1 and Mac OS X
#
.PHONY: all clean run

##---------------------------------------------------------------------
## BUILD RULES
##---------------------------------------------------------------------

all:
	$(MAKE) -C src

clean:
	$(MAKE) -C src clean

run:
	$(MAKE) -C src run
