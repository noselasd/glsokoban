CFLAGS = -g -Wall `sdl-config --cflags`
LIBS = -lGL -lGLU -lSDL_image `sdl-config --libs` -lm
CC= gcc
PROG_NAME=glsokoban

C_FILES=camera.c  ftfont.c frame.c init.c input.c levels.c main.c material.c maincamera.c quat.c\
	textures.c timer.c vector.c sky.c
O_FILES=$(C_FILES:%.c=%.o)
H_FILES= camera.h common.h init.h input.h material.h maincamera.h quat.h sokoban.h timer.h vector.h

all: depend $(PROG_NAME) 

$(PROG_NAME): $(O_FILES) LucidaSR.font
	$(CC) $(CFLAGS)  -o $@ $(O_FILES) $(LIBS)

fonttool: 
	$(CC) -std=gnu99 $(CFLAGS) `freetype-config --cflags`  -o fonttool.o -c fonttool.c
	$(CC) `freetype-config --libs` -o $@  fonttool.o
	
LucidaSR.font: fonttool
	./fonttool LucidaSansRegular.ttf LucidaSR.font 28

%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $< 

clean:
	rm -f $(O_FILES) $(PROG_NAME) depend fonttool fonttool.o

depend: $(C_FILES) $(H_FILES)
	$(CC) $(CFLAGS) -MM $(C_FILES)  >depend

include depend

.PHONY: all clean
