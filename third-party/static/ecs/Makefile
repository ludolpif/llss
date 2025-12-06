#
# Cross Platform Makefile
# Compatible with MSYS2/MINGW, Ubuntu 14.04.1 and Mac OS X
#
.PHONY: all clean

CFLAGS += `./cflags`

##---------------------------------------------------------------------
## BUILD RULES
##---------------------------------------------------------------------

all: flecs.o

flecs.o: flecs/flecs.c
	$(COMPILE.c) $(OUTPUT_OPTION) $<

clean:
	rm -f flecs.o
