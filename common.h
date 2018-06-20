#ifndef __COMMON_H__
#define __COMMON_H__
#include <stdio.h>
#include <SDL/SDL.h>
#include "timer.h"

#define DEBUG(x,...) fprintf(stderr,x,__VA_ARGS__);

#define NEAR_PLANE 1.0f
#define FOV 25.0f
#define FAR_PLANE 302.0f
#define HEIGHT 600
#define WIDTH 800
#ifndef MAINDIR
#define MAINDIR "./"
#endif



void do_quit(int code);
void frame_begin(int moves, int finished);
void frame_init(void);
void ftPrint(const char *str, float x, float y, float size);
extern SDL_Surface *mainWindow;
extern Timer frameTimer;
void init_fonts(void);
void calc_distance(void);
void frame_newlevel(void);


#endif
