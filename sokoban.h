#ifndef __SOKOBAN_H__
#define __SOKOBAN_H__

enum BrickType {
	Empty,
	Background,
	Wall,
	Cargo,
	Goal,
	GoalCargo
};
typedef struct {
	int x, y;
} Point;
#define MAX_X 30
#define MAX_Y 30

typedef struct {
	Point pos;
	int lvlnr;
	int width;
	int height;
	enum BrickType board[MAX_X][MAX_Y];
} Level;

typedef struct {
	int nrlevels;
	char *name;
	Level **levels;
} LevelSet;
enum Direction {
	UP, DOWN, LEFT, RIGHT
};

enum TexName {
	Brick,
	Sky,
	NumTextures
};

int load_levels(const char *filename);
void load_textures(void);
extern GLuint textures[NumTextures];
extern Level currentlevel;
int set_level(int nr);
int next_level(void);
int move(enum Direction d);
int restart_level(void);
int check_goal(void);
Level *current_level(void);

#endif
