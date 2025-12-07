#
# Cross Platform Makefile
# Compatible with MSYS2/MINGW, Ubuntu 14.04.1 and Mac OS X
#
DIRS := $(shell dirname */src)

.PHONY: all clean $(DIRS)

##---------------------------------------------------------------------
## BUILD RULES
##---------------------------------------------------------------------
# targets: normal-prerequisites | order-only-prerequisites

all: $(DIRS)

$(DIRS):
	$(MAKE) -C $@/src

clean:
	for dir in $(DIRS); do $(MAKE) -C $$dir/src clean; done
