#
# Cross Platform Makefile
# Compatible with MSYS2/MINGW, Ubuntu 14.04.1 and Mac OS X
#
.PHONY: all clean

CXXFLAGS = -g -Wall -Wformat -O0 -std=c++11
IMGUI_DIR=imgui
DCIMGUI_DIR=dear_bindings/generated

EXE = libdcimgui.a
SOURCES = $(IMGUI_DIR)/imgui.cpp $(IMGUI_DIR)/imgui_demo.cpp $(IMGUI_DIR)/imgui_draw.cpp $(IMGUI_DIR)/imgui_tables.cpp $(IMGUI_DIR)/imgui_widgets.cpp
SOURCES += $(IMGUI_DIR)/backends/imgui_impl_sdl3.cpp $(IMGUI_DIR)/backends/imgui_impl_sdlgpu3.cpp
SOURCES += $(DCIMGUI_DIR)/dcimgui.cpp $(DCIMGUI_DIR)/backends/dcimgui_impl_sdl3.cpp $(DCIMGUI_DIR)/backends/dcimgui_impl_sdlgpu3.cpp
OBJS = $(addsuffix .o, $(basename $(notdir $(SOURCES))))

CXXFLAGS += `./pkgc --cflags`
LIBS = `./pkgc --libs`

##---------------------------------------------------------------------
## BUILD RULES
##---------------------------------------------------------------------

%.o:$(IMGUI_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

%.o:$(IMGUI_DIR)/backends/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

%.o:$(DCIMGUI_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

%.o:$(DCIMGUI_DIR)/backends/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

all: $(EXE)

$(EXE): $(OBJS)
	ar rcs $(EXE) $(OBJS)

clean:
	rm -f $(OBJS) $(EXE)
