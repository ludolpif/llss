#
# Cross Platform Makefile
# Compatible with MSYS2/MINGW, GNU/Linux and Mac OS X
#
.PHONY: all clean symdump

SolutionDir=../../../

CC := gcc
#CPPFLAGS +=
CFLAGS += `./cflags`
LDFLAGS += $(shell ./ldflags)
LDLIBS += $(shell ./ldlibs)

EXE := ../$(shell ./soname)
# Makefile.autodeps needs $(SRCS)
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
$(OBJS): $(SolutionDir)third-party/ui/libdcimgui.a $(SolutionDir)third-party/ecs/flecs.o

$(SolutionDir)third-party/ecs/flecs.o:
	$(MAKE) -C $(SolutionDir)third-party/ecs

$(SolutionDir)third-party/ui/libdcimgui.a:
	$(MAKE) -C $(SolutionDir)third-party/ui

clean:
	rm -f $(EXE) $(OBJS)
	rm -rf .deps

symdump: $(EXE)
	objdump -T $(EXE)

include $(SolutionDir)Makefile.autodeps
