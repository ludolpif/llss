#
# Cross Platform Makefile
# Compatible with MSYS2/MINGW, GNU/Linux and Mac OS X
#
.PHONY: all clean symdump

CC := gcc
#CPPFLAGS +=
CFLAGS += -g -Wall -Wformat -O0
CFLAGS += `./cflags`
LDFLAGS += $(shell ./ldflags)
LDLIBS += $(shell ./ldlibs)

EXE := ../$(shell ./soname)
# ../../../Makefile.autodeps needs $(SRCS)
SRCS := $(wildcard *.c)
OBJS := $(SRCS:.c=.o)

##---------------------------------------------------------------------
## BUILD RULES
##---------------------------------------------------------------------
# targets: normal-prerequisites | order-only-prerequisites

all: $(EXE)

$(EXE): $(OBJS)
	$(LINK.c) $^ $(LDLIBS) -o $@
# LINK.c = $(CC) $(CFLAGS) $(CPPFLAGS) $(LDFLAGS) $(TARGET_ARCH)

# Prevent rule '%.o : %.c' to start before imgui submodule is available
$(OBJS): ../../../third-party/static/ui/libdcimgui.a ../../../third-party/static/ecs/flecs.o

../../../third-party/static/ecs/flecs.o:
	$(MAKE) -C ../../../third-party/static/ecs

../../../third-party/static/ui/libdcimgui.a:
	$(MAKE) -C ../../../third-party/static/ui

clean:
	rm -f $(EXE) $(OBJS)

symdump: $(EXE)
	objdump -T $(EXE)

include ../../../Makefile.autodeps
