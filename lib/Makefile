#
# Cross Platform Makefile
# Compatible with MSYS2/MINGW, Ubuntu 14.04.1 and Mac OS X
#
DIRS := $(shell dirname */Makefile)

.PHONY: all clean $(DIRS)

##---------------------------------------------------------------------
## BUILD RULES
##---------------------------------------------------------------------

all: $(DIRS)

$(DIRS):
	$(MAKE) -C $@

clean:
	for dir in $(DIRS); do $(MAKE) -C $$dir clean; done
