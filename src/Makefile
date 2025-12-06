#
# Cross Platform Makefile
# Compatible with MSYS2/MINGW, GNU/Linux and Mac OS X
#
.PHONY: all clean clean-recursive run

CC := gcc
#CPPFLAGS +=
CFLAGS += `./cflags`
LDFLAGS += $(shell ./ldflags)
LDLIBS += $(shell ./ldlibs)

EXE = ../app
# Makefile.autodeps needs $(SRCS)
SRCS := $(wildcard *.c)
OBJS := $(SRCS:.c=.o)

##---------------------------------------------------------------------
## BUILD RULES
##---------------------------------------------------------------------
# targets: normal-prerequisites | order-only-prerequisites

all: $(EXE)

$(EXE): $(OBJS)
	$(LINK.c) $(OBJS) ../lib/ecs/flecs.o $(LDLIBS) -o $@
# LINK.c = $(CC) $(CFLAGS) $(CPPFLAGS) $(LDFLAGS) $(TARGET_ARCH)

# Prevent rule '%.o : %.c' to start before imgui submodule is available
$(OBJS): ../lib/ui/libdcimgui.a ../lib/ecs/flecs.o

../lib/ecs/flecs.o:
	$(MAKE) -C ../lib/ecs

../lib/ui/libdcimgui.a:
	$(MAKE) -C ../lib/ui

clean:
	rm -f $(EXE) $(OBJS)
	rm -rf .deps

include ../Makefile.autodeps
