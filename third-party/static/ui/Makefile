#
# Cross Platform Makefile
# Compatible with MSYS2/MINGW, Ubuntu 14.04.1 and Mac OS X
#
.PHONY: all clean

CXXFLAGS = -g -Wall -Wformat -O0 -std=c++11

EXE = libdcimgui.a
SOURCES = imgui/imgui.cpp imgui/imgui_demo.cpp imgui/imgui_draw.cpp imgui/imgui_tables.cpp imgui/imgui_widgets.cpp
SOURCES += imgui/backends/imgui_impl_sdl3.cpp imgui/backends/imgui_impl_sdlgpu3.cpp
SOURCES += dear_bindings_generated/dcimgui.cpp
SOURCES += dear_bindings_generated/backends/dcimgui_impl_sdl3.cpp
SOURCES += dear_bindings_generated/backends/dcimgui_impl_sdlgpu3.cpp
OBJS := $(addsuffix .o, $(basename $(notdir $(SOURCES))))

CXXFLAGS += `./pkgc --cflags`
LIBS = `./pkgc --libs`

##---------------------------------------------------------------------
## BUILD RULES
##---------------------------------------------------------------------

%.o: imgui/%.cpp dear_bindings_generated.timestamp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

%.o: imgui/backends/%.cpp dear_bindings_generated.timestamp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

%.o: dear_bindings_generated/%.cpp dear_bindings_generated.timestamp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

%.o: dear_bindings_generated/backends/%.cpp dear_bindings_generated.timestamp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

all: $(EXE)

$(EXE): $(OBJS)
	ar rcs $(EXE) $(OBJS)

clean:
	rm -f $(OBJS) $(EXE)

# TODO use imconfig_user.h ?

dear_bindings_generated.timestamp: dear_bindings/dear_bindings.py
	mkdir -p dear_bindings_generated/backends
	cd dear_bindings; python3 dear_bindings.py -o ../dear_bindings_generated/dcimgui ../imgui/imgui.h
	cd dear_bindings; python3 dear_bindings.py -o ../dear_bindings_generated/dcimgui_internal --include ../imgui/imgui.h ../imgui/imgui_internal.h
	cd dear_bindings; python3 dear_bindings.py --backend --include ../imgui/imgui.h --imconfig-path ../imgui/imconfig.h -o ../dear_bindings_generated/backends/dcimgui_impl_sdl3 ../imgui/backends/imgui_impl_sdl3.h
	cd dear_bindings; python3 dear_bindings.py --backend --include ../imgui/imgui.h --imconfig-path ../imgui/imconfig.h -o ../dear_bindings_generated/backends/dcimgui_impl_sdlgpu3 ../imgui/backends/imgui_impl_sdlgpu3.h
	touch dear_bindings_generated.timestamp

dear_bindings/dear_bindings.py:
	git submodule update --init --recursive

