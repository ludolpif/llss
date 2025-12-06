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

MOD_SO := ../$(shell ./soname)
# Makefile.autodeps needs $(SRCS), not easly compatible with sources in subfolders
SRCS := $(wildcard *.c)
OBJS := $(SRCS:.c=.o)

##---------------------------------------------------------------------
## BUILD RULES
##---------------------------------------------------------------------

all: $(MOD_SO)

$(MOD_SO): $(OBJS)
	$(LINK.c) $^ $(LDLIBS) -o $@
# LINK.c = $(CC) $(CFLAGS) $(CPPFLAGS) $(LDFLAGS) $(TARGET_ARCH)

clean:
	rm -f $(MOD_SO) $(OBJS)
	rm -rf .deps

symdump: $(MOD_SO)
	objdump -T $(MOD_SO)

include $(SolutionDir)Makefile.autodeps
