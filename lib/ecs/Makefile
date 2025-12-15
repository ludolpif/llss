#
# Cross Platform Makefile
# Compatible with MSYS2/MINGW, Ubuntu 14.04.1 and Mac OS X
#
.PHONY: all clean

# default value for command-line arguments (like make BUILD_TYPE=RelWithDebInfo)
BUILD_TYPE ?= Debug

ODIR := x64/$(BUILD_TYPE)/

CC := gcc
CFLAGS += `./cflags $(BUILD_TYPE)`

##---------------------------------------------------------------------
## BUILD RULES
##---------------------------------------------------------------------
# targets: normal-prerequisites | order-only-prerequisites

all: $(ODIR)flecs.o

$(ODIR)flecs.o: flecs/flecs.c | $(ODIR)
	$(COMPILE.c) -o $@ $<
# COMPILE.c = $(CC) $(CFLAGS) $(CPPFLAGS) $(TARGET_ARCH) -c

$(ODIR):
	@mkdir -p $(ODIR)

clean:
	rm -f $(ODIR)flecs.o
