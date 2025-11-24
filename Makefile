#
# Cross Platform Makefile
# Compatible with MSYS2/MINGW, Ubuntu 14.04.1 and Mac OS X
#
.PHONY: all clean

CFLAGS = -g -Wall -Wformat -O0

EXE = llss
SOURCES=src/main.c
OBJS = $(addsuffix .o, $(basename $(notdir $(SOURCES))))
DCIMGUI_BUILDDIR=third-party/static/ui

CFLAGS += `./pkgc --cflags`
LIBS = `./pkgc --libs`

##---------------------------------------------------------------------
## BUILD RULES
##---------------------------------------------------------------------

%.o:src/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

all: $(EXE)

$(EXE): $(OBJS) third-party/static/ui/dcimgui.a
	$(CC) -o $@ $(OBJS) $(LIBS)

third-party/static/ui/dcimgui.a:
	$(MAKE) -C third-party/static/ui

clean:
	rm -f $(EXE) $(OBJS)
	$(MAKE) -C third-party/static/ui clean
