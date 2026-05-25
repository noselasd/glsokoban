#ifndef __SOKOBAN_H__
#define __SOKOBAN_H__

#if defined(__APPLE__)
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif

#include "larena.h"

enum TileType { Empty, Background, Wall, Cargo, Goal, GoalCargo };
typedef int8_t Tile;
typedef struct {
    int x, y;
} Point;

static inline Point Pt(int x, int y)
{
    Point p = {x, y};
    return p;
}

#define MAX_X 30
#define MAX_Y 30

typedef struct {
    Point playerPos;
    int lvlnr;
    int width;
    int height;
    Tile board[MAX_X][MAX_Y];
} Level;

typedef struct {
    char *name;
    int nrlevels;
    Level *levels;
} LevelSet;
enum Direction { UP, DOWN, LEFT, RIGHT };

enum TexName { Brick, Sky, NumTextures };

#define GAME_ARENA_SZ (1024*1000)

typedef struct {
    Level currentLevel;
    LevelSet levels;
    int nr_moves;
    GLuint textures[NumTextures];
    LArena *arena;
} GameData;

void init_game(GameData *game, LArena *arena);
int load_levels(GameData *game, const char *filename);
void load_textures(GameData *game);
extern GLuint textures[NumTextures];
int set_level(GameData *game, int nr);
int next_level(GameData *game);
int move(Level *level, enum Direction d);
int restart_level(GameData *game);
int check_goal(Level *level);

#endif
