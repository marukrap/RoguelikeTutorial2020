# This is a makefile for Microsoft nmake

EMCC = emcc

CPPFLAGS = \
	-I../Sources \
	-std=c++17 \
	-Wall -O2 \

EMFLAGS = \
	-s USE_SDL=2 \
	-s USE_SDL_IMAGE=2 \
	-s USE_SDL_TTF=2 \
	-s SDL2_IMAGE_FORMATS=["png"] \
	--preload-file Fonts \
	--shell-file shell_minimal.html \

SOURCES = \
	../Sources/Engine/*.cpp \

TARGETS = Part1 Part2 Part3 Part4 Part5

all : $(TARGETS)

$(TARGETS) : $(SOURCES) ../Sources/$@/*.cpp
	$(EMCC) -I../Sources/$@ $(CPPFLAGS) $(EMFLAGS) $** -o $@.html

clean :
	del /f Part*.data Part*.html Part*.js Part*.wasm
