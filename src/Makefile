#
# Cross Platform Makefile
# Compatible with MSYS2/MINGW, GNU/Linux and Mac OS X
#
.PHONY: all clean clean-recursive run

CC := gcc
#CPPFLAGS +=
CFLAGS += -g -Wall -Wformat -O0 
CFLAGS += `./cflags`
LDFLAGS += $(shell ./ldflags)
LDLIBS += $(shell ./ldlibs)

EXE = ../llss
# ../Makefile.autodeps needs $(SRCS)
SRCS := $(wildcard *.c)
OBJS := $(SRCS:.c=.o)

##---------------------------------------------------------------------
## BUILD RULES
##---------------------------------------------------------------------
# targets: normal-prerequisites | order-only-prerequisites

all: $(EXE)

$(EXE): $(OBJS) ../third-party/static/ecs/flecs.o ../third-party/static/ui/libdcimgui.a
	$(LINK.c) $(OBJS) ../third-party/static/ecs/flecs.o $(LDLIBS) -o $@
# LINK.c = $(CC) $(CFLAGS) $(CPPFLAGS) $(LDFLAGS) $(TARGET_ARCH)

# Prevent rule '%.o : %.c' to start before imgui submodule is available
$(OBJS): ../third-party/static/ui/imgui/imgui.h

../third-party/static/ecs/flecs.o:
	$(MAKE) -C ../third-party/static/ecs

../third-party/static/ui/libdcimgui.a:
	$(MAKE) -C ../third-party/static/ui

../third-party/static/ui/imgui/imgui.h:
	git submodule update --init --recursive ../third-party/static/ui/imgui

clean:
	rm -f $(EXE) $(OBJS)

clean-recursive:
	$(MAKE) -C ../clean-recursive

include ../Makefile.autodeps
