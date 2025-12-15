#
# Cross Platform Makefile
# Compatible with MSYS2/MINGW, GNU/Linux and Mac OS X
#
.PHONY: all clean symdump

# default value for command-line arguments (like make BUILD_TYPE=RelWithDebInfo)
BUILD_TYPE ?= Debug

ODIR := x64/$(BUILD_TYPE)/
DEPDIR := $(ODIR).deps

CC := gcc
#CPPFLAGS +=
CFLAGS += `./cflags $(BUILD_TYPE)`
LDFLAGS += $(shell ./ldflags $(BUILD_TYPE))
LDLIBS += $(shell ./ldlibs $(BUILD_TYPE))
EXE = $(ODIR)$(shell ./exename $(BUILD_TYPE))

# Makefile.autodeps needs $(SRCS)
SRCS := $(wildcard *.c)
OBJS := $(SRCS:%.c=$(ODIR)%.o)

##---------------------------------------------------------------------
## BUILD RULES
##---------------------------------------------------------------------

all: $(EXE)

$(EXE): $(OBJS)
	$(LINK.c) $(OBJS) ../lib/ecs/$(ODIR)flecs.o $(LDLIBS) -o $@
# LINK.c = $(CC) $(CFLAGS) $(CPPFLAGS) $(LDFLAGS) $(TARGET_ARCH)

clean:
	rm -f $(EXE) $(OBJS)
	rm -rf $(DEPDIR)

symdump: $(MOD_SO)
	objdump -T $(MOD_SO)

include ../Makefile.autodeps
