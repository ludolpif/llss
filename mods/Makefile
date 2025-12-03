#
# Cross Platform Makefile
# Compatible with MSYS2/MINGW, Ubuntu 14.04.1 and Mac OS X
#
MODS := $(shell dirname */src)
MODS_SO := $(foreach dir,$(MODS),$(shell cd $(dir)/src && ./soname))

.PHONY: all clean $(MODS)

all: $(MODS)

$(MODS):
	$(MAKE) -C $@/src

clean:
	for mod in $(MODS); do $(MAKE) -C $$mod/src clean; done
