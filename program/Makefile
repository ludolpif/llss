#
# Cross Platform Makefile
# Compatible with MSYS2/MINGW, GNU/Linux and Mac OS X
#
.PHONY: all clean run symdump

# default value for command-line arguments (like make BUILD_TYPE=Release)
BUILD_TYPE ?= Debug

ODIR := x64/$(BUILD_TYPE)/
DEPDIR := $(ODIR).deps

CC := gcc
#CPPFLAGS +=
CFLAGS += `./get --cflags $(BUILD_TYPE)`
LDFLAGS += $(shell ./get --ldflags $(BUILD_TYPE))
LDLIBS += $(shell ./get --ldlibs $(BUILD_TYPE))
EXE = $(ODIR)$(shell ./get --exename $(BUILD_TYPE))

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

symdump: $(EXE)
	objdump -T $(EXE)

run: $(EXE)
	SDL_LOGGING="app=trace,assert=warn,test=verbose,*=error" $(EXE) 2>&1 | ../dev/pretty-log.pl

include ../Makefile.autodeps
