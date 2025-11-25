#
# Cross Platform Makefile
# Compatible with MSYS2/MINGW, Ubuntu 14.04.1 and Mac OS X
#
.PHONY: all clean clean-recursive run

CFLAGS = -g -Wall -Wformat -O0 -std=c11

EXE = ./llss
SOURCES := $(wildcard *.c)
OBJS := $(addsuffix .o, $(basename $(notdir $(SOURCES))))

CFLAGS += `./pkgc --cflags`
LIBS = `./pkgc --libs`

##---------------------------------------------------------------------
## BUILD RULES
##---------------------------------------------------------------------

#TODO ABI problems between modules appears if headers changes and make isn't told to recompile
%.o: %.c ../include/app.h
	$(CC) $(CFLAGS) -c -o $@ $<

all: $(EXE)

$(EXE): $(OBJS) ../third-party/static/ui/dcimgui.a
	$(CC) -o $@ $(OBJS) $(LIBS)

../third-party/static/ui/dear_bindings_generated.timestamp:
	$(MAKE) -C ../third-party/static/ui

../third-party/static/ui/dcimgui.a:
	$(MAKE) -C ../third-party/static/ui

clean:
	rm -f $(EXE) $(OBJS)

clean-recursive: clean
	$(MAKE) -C ../third-party/static/ui clean

run: $(EXE)
	SDL_LOGGING="app=info,assert=warn,test=verbose,*=error" $(EXE)

run-app-trace: $(EXE)
	SDL_LOGGING="app=trace,assert=warn,test=verbose,*=error" $(EXE)

run-all-trace: $(EXE)
	SDL_LOGGING="*=trace" $(EXE)
