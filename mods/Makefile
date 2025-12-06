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

all: $(DIRS) #| libdcimgui.so

#libdcimgui.so: ../lib/ui/so/libdcimgui.so
#	ln -nsf ../lib/ui/so/libdcimgui.so

$(DIRS):
	$(MAKE) -C $@/src

clean:
	for dir in $(DIRS); do $(MAKE) -C $$dir/src clean; done
