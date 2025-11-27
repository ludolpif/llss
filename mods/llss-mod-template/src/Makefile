#
# Cross Platform Makefile
# Compatible with MSYS2/MINGW, Ubuntu 14.04.1 and Mac OS X
#
.PHONY: all clean symdump

CFLAGS = -g -Wall -Wformat -O0 -std=gnu99

EXE := $(shell ./pkgc --soname)

SOURCES := $(wildcard *.c)
OBJS := $(addsuffix .o, $(basename $(notdir $(SOURCES))))

CFLAGS += -fpic
CFLAGS += `./pkgc --cflags`
LIBS = `./pkgc --libs`

##---------------------------------------------------------------------
## BUILD RULES
##---------------------------------------------------------------------

#TODO ABI problems between modules appears if headers changes and make isn't told to recompile
%.o: %.c ../../../include/app.h
	$(CC) $(CFLAGS) -c -o $@ $< -fvisibility=hidden

all: $(EXE)

$(EXE): $(OBJS)
	$(CC) -o $@ $(OBJS) $(LIBS) -Wl,--default-symver -Wl,-soname,$(EXE) -shared

clean:
	rm -f $(EXE) $(OBJS)

symdump: $(EXE)
	objdump -T $(EXE)
