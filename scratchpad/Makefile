#
# This file is part of LLSS.
#
# LLSS is free software: you can redistribute it and/or modify it under the terms of the
# Affero GNU General Public License as published by the Free Software Foundation,
# either version 3 of the License, or (at your option) any later version.
#
# LLSS is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
# without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License along with LLSS.
# If not, see <https://www.gnu.org/licenses/>. See LICENSE file at root of this git repo.
#
# Copyright 2025 ludolpif <ludolpif@gmail.com>
#
# For now, build for Windows and Mac uses VSCode MSBuild and XCode tools
.PHONY: all clean run seergdb valgrind

# default value for command-line arguments (like make BUILD_TYPE=Release)
BUILD_TYPE ?= Debug

ODIR := x64/$(BUILD_TYPE)/
DEPDIR := $(ODIR).deps

CC := gcc
#CPPFLAGS +=
CFLAGS += `./get --cflags $(BUILD_TYPE)`
LDFLAGS += $(shell ./get --ldflags $(BUILD_TYPE))
LDLIBS += $(shell ./get --ldlibs $(BUILD_TYPE))
EXE = $(ODIR)scratch

# Makefile.autodeps needs $(SRCS)
SRCS := $(wildcard *.c)
OBJS := $(SRCS:%.c=$(ODIR)%.o)

GDB_BREAKFUNC ?= init_hook
RUN_ARGS ?= 

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

run:
	$(EXE)

seergdb:
	seergdb --bf $(GDB_BREAKFUNC) --cwd . --run $(EXE) $(RUN_ARGS)

valgrind:
	valgrind $(EXE) $(RUN_ARGS)

valgrind-full:
	valgrind --leak-check=full $(EXE) $(RUN_ARGS)

include ../Makefile.autodeps
