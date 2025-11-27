#
# Cross Platform Makefile
# Compatible with MSYS2/MINGW, GNU/Linux and Mac OS X
#
.PHONY: all clean clean-recursive run

CC := gcc
#CPPFLAGS +=
CFLAGS += -g -Wall -Wformat -O0 
CFLAGS += -std=gnu99 `./cflags`
LDFLAGS += $(shell ./ldflags)
LDLIBS += $(shell ./ldlibs)

# ../Makefile.autodeps needs $(SRCS)
# Don't list llss.c as we use Make implicit rule "binary: obj1.o obj2.o" which auto-build binary.o
SRCS := alloc.c app-event.c app-init.c app-iterate.c mod-host.c ui-main.c
OBJS := $(SRCS:.c=.o)

all: llss
	
# targets : normal-prerequisites | order-only-prerequisites
llss: $(OBJS) ../third-party/static/ecs/flecs.o | ../third-party/static/ui/libdcimgui.a ../third-party/static/ui/imgui/imgui.h

../third-party/static/ecs/flecs.o:
	$(MAKE) -C ../third-party/static/ecs

../third-party/static/ui/libdcimgui.a:
	$(MAKE) -C ../third-party/static/ui

../third-party/static/ui/imgui/imgui.h:
	git submodule update --init --recursive ../third-party/static/ui/imgui

clean:
	rm -f llss $(OBJS)

clean-recursive:
	$(MAKE) -C ../clean-recursive

 include ../Makefile.autodeps
