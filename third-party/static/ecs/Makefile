#
# Cross Platform Makefile
# Compatible with MSYS2/MINGW, Ubuntu 14.04.1 and Mac OS X
#
.PHONY: all clean

CFLAGS = -g -Wall -Wformat -std=gnu99

##---------------------------------------------------------------------
## BUILD RULES
##---------------------------------------------------------------------

flecs.o: flecs/flecs.c
	$(CC) $(CFLAGS) -Iflecs -c -o $@ $<

all: flecs.o

clean:
	rm -f flecs.o
