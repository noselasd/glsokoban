ifeq ($(OS),Windows_NT)     # is Windows_NT on XP, 2000, 7, Vista, 10...
    detected_OS := Windows
else
    detected_OS := $(shell uname)
endif
DEbUG=1

CFLAGS =  -g -Wall -Wextra -Wno-switch `sdl2-config --cflags`
LIBS =   `sdl2-config --libs` -lSDL2_image  -lm
FONTTOOL_CFLAGS = `pkg-config --cflags freetype2`
FONTTOOL_LIBS = `pkg-config --libs freetype2`
CC = gcc
PROG_NAME=glsokoban

ifeq (DEBUG,1)
	CFLAGS += -fsanitize=address
	LIBS += -fsanitize=address
	FONTTOOL_CFLAGS += -fsanitize=address
	FONTTOOL_LIBS += -fsanitize=address
else
	CFLAGS += -O3 -DNDEBUG
endif

ifeq ($(detected_OS),Darwin)        # Mac OS X
    CFLAGS += -DGL_SILENCE_DEPRECATION
	LIBS += -framework OpenGL
	CC = clang
else
    LIBS += -lGL -lGL -lGLU
endif

C_FILES=camera.c  ftfont.c frame.c init.c input.c levels.c main.c material.c maincamera.c quat.c\
	textures.c timer.c vector.c sky.c
O_FILES=$(C_FILES:%.c=%.o)
H_FILES= camera.h common.h init.h input.h material.h maincamera.h quat.h sokoban.h timer.h vector.h

all: depend $(PROG_NAME)

$(PROG_NAME): $(O_FILES) LucidaSR.font
	$(CC) $(CFLAGS)  -o $@ $(O_FILES) $(LIBS)

fonttool : fonttool.c
	$(CC) -std=gnu99 $(CFLAGS) $(FONTTOOL_CFLAGS)  -o fonttool.o -c fonttool.c
	$(CC) $(FONTTOOL_LIBS) -lm -o $@  fonttool.o

LucidaSR.font: fonttool
	./fonttool LucidaSansRegular.ttf LucidaSR.font 28

%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<

clean:
	rm -f $(O_FILES) $(PROG_NAME) depend fonttool fonttool.o

depend: $(C_FILES) $(H_FILES)
	$(CC) $(CFLAGS) -MM $(C_FILES)  >depend

# info for clangd language server
compile_commands.json: Makefile
	bear -- $(MAKE) clean all
ifneq ($(filter clean,$(MAKECMDGOALS)),clean)
include depend
endif

.PHONY: all clean
