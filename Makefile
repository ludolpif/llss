#
# Cross Platform Makefile
# Compatible with MSYS2/MINGW, Ubuntu 14.04.1 and Mac OS X
#
.PHONY: all clean clean-recursive run

##---------------------------------------------------------------------
## BUILD RULES
##---------------------------------------------------------------------

all:
	$(MAKE) -C src

clean:
	$(MAKE) -C src clean

clean-recursive:
	$(MAKE) -C src clean-recursive

run:
	$(MAKE) -C src run
