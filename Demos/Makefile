# This is a makefile for Microsoft nmake

EMCC = emcc

CPPFLAGS = \
	-I../Sources \
	-std=c++17 \
	-Wall -O3 \

EMFLAGS = \
	-lidbfs.js	\
	-s EXTRA_EXPORTED_RUNTIME_METHODS="['ccall', 'cwrap']" \
	-s EXPORTED_FUNCTIONS="['_main', '_initFS', '_onSave']" \
	-s USE_SDL=2 \
	-s USE_SDL_IMAGE=2 \
	-s USE_SDL_TTF=2 \
	-s SDL2_IMAGE_FORMATS=["png"] \
	-s ALLOW_MEMORY_GROWTH=1 \
	--no-heap-copy \
	--preload-file Fonts \
	--shell-file shell_minimal.html \

SOURCES = \
	../Sources/Engine/*.cpp \

TARGETS = Part1 Part2 Part3 Part4 Part5 Part6 Part7 Part8 Part9
TARGETS2 = Part10 Part11 Part12 Part13

all : $(TARGETS) $(TARGETS2)

$(TARGETS) : $(SOURCES) ../Sources/$@/*.cpp
	$(EMCC) -I../Sources/$@ $(CPPFLAGS) $(EMFLAGS) $** -o $@.html

$(TARGETS2) : $(SOURCES) ../Sources/$@/*.cpp ../Sources/$@/Entity/*.cpp ../Sources/$@/Menu/*.cpp
	$(EMCC) -I../Sources/$@ $(CPPFLAGS) $(EMFLAGS) $** -o $@.html

clean :
	del /f Part*.data Part*.html Part*.js Part*.wasm
