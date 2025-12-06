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
	$(LINK.c) $(OBJS) ../third-party/ecs/flecs.o $(LDLIBS) -o $@
# LINK.c = $(CC) $(CFLAGS) $(CPPFLAGS) $(LDFLAGS) $(TARGET_ARCH)

# Prevent rule '%.o : %.c' to start before imgui submodule is available
$(OBJS): ../third-party/ui/libdcimgui.a ../third-party/ecs/flecs.o

../third-party/ecs/flecs.o:
	$(MAKE) -C ../third-party/ecs

../third-party/ui/libdcimgui.a:
	$(MAKE) -C ../third-party/ui

clean:
	rm -f $(EXE) $(OBJS)
	rm -rf .deps

include ../Makefile.autodeps
