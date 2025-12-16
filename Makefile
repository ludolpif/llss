#
# Cross Platform Makefile
# Compatible with MSYS2/MINGW, Ubuntu 14.04.1 and Mac OS X
#
.PHONY: all clean program mods lib run run-program-trace run-all-trace

# default value for command-line arguments (like make BUILD_TYPE=Release)
BUILD_TYPE ?= Debug

all: program mods

# ordering rule. if dependencies are expressed in subdirs Makefile then calling here a 
# "make -j" can call $(MAKE) -C lib multiple times, concurrently and it will fail
program mods: lib

program:
	$(MAKE) -C program BUILD_TYPE=$(BUILD_TYPE)

mods:
	$(MAKE) -C mods BUILD_TYPE=$(BUILD_TYPE)

lib:
	$(MAKE) -C lib BUILD_TYPE=$(BUILD_TYPE)

clean:
	$(MAKE) -C program clean BUILD_TYPE=$(BUILD_TYPE)
	$(MAKE) -C mods clean BUILD_TYPE=$(BUILD_TYPE)
	$(MAKE) -C lib clean BUILD_TYPE=$(BUILD_TYPE)

run: all
	$(MAKE) -C program run BUILD_TYPE=$(BUILD_TYPE)
