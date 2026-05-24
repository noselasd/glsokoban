#ifndef __COMMON_H__
#define __COMMON_H__
#include <SDL.h>
#include "sokoban.h"
#include "timer.h"

#define DEBUG(x, ...) fprintf(stderr, x, __VA_ARGS__);

#define NEAR_PLANE 1.0f
#define FOV 25.0f
#define FAR_PLANE 302.0f
#define HEIGHT 600
#define WIDTH 800
#ifndef MAINDIR
#define MAINDIR "./"
#endif

void do_quit(int code);
void frame_begin(GameData *game, int finished);
void frame_init(GameData *game);
void ftPrint(const char *str, float x, float y, float size);
extern SDL_Window *mainWindow;
extern Timer frameTimer;
void init_fonts(void);
void calc_distance(GameData *game);
void frame_newlevel(GameData *game);

#endif
