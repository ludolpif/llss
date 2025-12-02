#
# Cross Platform Makefile
# Compatible with MSYS2/MINGW, Ubuntu 14.04.1 and Mac OS X
#
.PHONY: all bindings clean

CXX := g++
#CPPFLAGS +=
CXXFLAGS += -g -O3
CXXFLAGS += `./cflags`
ARFLAGS = rcs

OBJS := imgui.o imgui_demo.o imgui_draw.o imgui_tables.o imgui_widgets.o imgui_impl_sdl3.o imgui_impl_sdlgpu3.o
OBJS += dcimgui.o dcimgui_impl_sdl3.o dcimgui_impl_sdlgpu3.o

VPATH := imgui:imgui/backends:dear_bindings_generated:dear_bindings_generated/backends

##---------------------------------------------------------------------
## BUILD RULES
##---------------------------------------------------------------------

all: libdcimgui.a

# Use GNU Make archive member syntax archive(member):
libdcimgui.a: libdcimgui.a($(OBJS))

# https://www.gnu.org/software/make/manual/html_node/Archive-Pitfalls.html
(%): %	;
%.a:
	$(AR) $(ARFLAGS) $@ $?

# Trigger full rebuild off all archive members on imgui_config.h change
(%.o): %.cpp ../../../include/imgui_config.h
	$(COMPILE.cpp) -o $% $<

clean:
	rm -f libdcimgui.a $(OBJS)

# targets : normal-prerequisites | order-only-prerequisites
bindings: | dear_bindings/dear_bindings.py
	mkdir -p dear_bindings_generated/backends
	cd dear_bindings; python3 dear_bindings.py -o ../dear_bindings_generated/dcimgui ../imgui/imgui.h
	cd dear_bindings; python3 dear_bindings.py -o ../dear_bindings_generated/dcimgui_internal --include ../imgui/imgui.h ../imgui/imgui_internal.h
	cd dear_bindings; python3 dear_bindings.py --backend --include ../imgui/imgui.h --imconfig-path ../imgui/imconfig.h -o ../dear_bindings_generated/backends/dcimgui_impl_sdl3 ../imgui/backends/imgui_impl_sdl3.h
	cd dear_bindings; python3 dear_bindings.py --backend --include ../imgui/imgui.h --imconfig-path ../imgui/imconfig.h -o ../dear_bindings_generated/backends/dcimgui_impl_sdlgpu3 ../imgui/backends/imgui_impl_sdlgpu3.h

dear_bindings/dear_bindings.py:
	git submodule update --init --recursive dear_bindings
