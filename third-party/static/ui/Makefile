#
# Cross Platform Makefile
# Compatible with MSYS2/MINGW, Ubuntu 14.04.1 and Mac OS X
#
.PHONY: all bindings clean

CC := gcc
#CPPFLAGS +=
CXXFLAGS += -g -O0
CXXFLAGS += -std=c++11 `./cflags`
ARFLAGS = rcs

EXE = libdcimgui.a
SRCS := imgui/imgui.cpp imgui/imgui_demo.cpp imgui/imgui_draw.cpp imgui/imgui_tables.cpp imgui/imgui_widgets.cpp
SRCS += imgui/backends/imgui_impl_sdl3.cpp imgui/backends/imgui_impl_sdlgpu3.cpp
SRCS += dear_bindings_generated/dcimgui.cpp
SRCS += dear_bindings_generated/backends/dcimgui_impl_sdl3.cpp
SRCS += dear_bindings_generated/backends/dcimgui_impl_sdlgpu3.cpp
OBJS := $(SRCS:.cpp=.o)

##---------------------------------------------------------------------
## BUILD RULES
##---------------------------------------------------------------------

all: $(EXE)

$(EXE): $(OBJS) ../../../include/imgui_config.h
	$(AR) $(ARFLAGS) $(EXE) $(OBJS)

clean:
	rm -f $(OBJS) $(EXE)

# targets : normal-prerequisites | order-only-prerequisites
bindings: | dear_bindings/dear_bindings.py
	mkdir -p dear_bindings_generated/backends
	cd dear_bindings; python3 dear_bindings.py -o ../dear_bindings_generated/dcimgui ../imgui/imgui.h
	cd dear_bindings; python3 dear_bindings.py -o ../dear_bindings_generated/dcimgui_internal --include ../imgui/imgui.h ../imgui/imgui_internal.h
	cd dear_bindings; python3 dear_bindings.py --backend --include ../imgui/imgui.h --imconfig-path ../imgui/imconfig.h -o ../dear_bindings_generated/backends/dcimgui_impl_sdl3 ../imgui/backends/imgui_impl_sdl3.h
	cd dear_bindings; python3 dear_bindings.py --backend --include ../imgui/imgui.h --imconfig-path ../imgui/imconfig.h -o ../dear_bindings_generated/backends/dcimgui_impl_sdlgpu3 ../imgui/backends/imgui_impl_sdlgpu3.h

dear_bindings/dear_bindings.py:
	git submodule update --init --recursive dear_bindings
