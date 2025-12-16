#
# Cross Platform Makefile
# Compatible with MSYS2/MINGW, GNU/Linux and Mac OS X
#
.PHONY: all clean symdump

# default value for command-line arguments (like make BUILD_TYPE=Release)
BUILD_TYPE ?= Debug

ODIR := x64/$(BUILD_TYPE)/
DEPDIR := $(ODIR).deps
SolutionDir := ../../../

CC := gcc
#CPPFLAGS +=
CFLAGS += `./get --cflags $(BUILD_TYPE)`
LDFLAGS += $(shell ./get --ldflags $(BUILD_TYPE))
LDLIBS += $(shell ./get --ldlibs $(BUILD_TYPE))

MOD_SO := $(ODIR)$(shell ./get --soname)
# Makefile.autodeps needs $(SRCS), not easly compatible with sources in subfolders
SRCS := $(wildcard *.c)
OBJS := $(SRCS:%.c=$(ODIR)%.o)

##---------------------------------------------------------------------
## BUILD RULES
##---------------------------------------------------------------------

all: $(MOD_SO)

$(MOD_SO): $(OBJS)
	$(LINK.c) $^ $(LDLIBS) -o $@
# LINK.c = $(CC) $(CFLAGS) $(CPPFLAGS) $(LDFLAGS) $(TARGET_ARCH)

clean:
	rm -f $(MOD_SO) $(OBJS)
	rm -rf $(DEPDIR)

symdump: $(MOD_SO)
	objdump -T $(MOD_SO)

include $(SolutionDir)Makefile.autodeps
