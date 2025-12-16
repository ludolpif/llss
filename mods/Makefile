#
# Cross Platform Makefile
# Compatible with MSYS2/MINGW, Ubuntu 14.04.1 and Mac OS X
#
DIRS := $(patsubst %/program,%,$(wildcard */program))

.PHONY: all clean $(DIRS)

##---------------------------------------------------------------------
## BUILD RULES
##---------------------------------------------------------------------

all: $(DIRS)

$(DIRS):
	$(MAKE) -C $@/program

clean:
	for dir in $(DIRS); do $(MAKE) -C $$dir/program clean; done
