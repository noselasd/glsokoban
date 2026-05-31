#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#if defined(__APPLE__)
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

#include "sokoban.h"
#include <assert.h>

#define MIN(a, b) (((a) < (b)) ? (a) : (b))
static Level nulllevel;

int check_goal(Level *level)
{
    int x, y;

    for (y = 0; y < level->height; y++) {
        for (x = 0; x < level->width; x++) {
            if (level->board[x][y] == Goal) return 0;
        }
    }

    return 1;
}
static void next_line(FILE *f)
{
    int c;
    while ((c = fgetc(f)) != EOF && c != '\n');
}

Tile get_tile(const Level *level, Point pt)
{
    if (pt.x < 0 || pt.x >= level->width) {
        return Wall;
    }
    if (pt.y < 0 || pt.y >= level->height) {
        return Wall;
    }

    return level->board[pt.x][pt.y];
}

int move_new(Level *level, enum Direction d)
{
    Point player = level->playerPos;
    Point player_next_pos;
    Point cargo_next_pos;

    if (d == UP) {
        player_next_pos = Pt(player.x, player.y - 1);
        cargo_next_pos = Pt(player.x, player.y - 2);
    } else if (d == DOWN) {
        player_next_pos = Pt(player.x, player.y + 1);
        cargo_next_pos = Pt(player.x, player.y + 2);
    } else if (d == LEFT) {
        player_next_pos = Pt(player.x - 1, player.y);
        cargo_next_pos = Pt(player.x - 2, player.y);
    } else if (d == RIGHT) {
        player_next_pos = Pt(player.x + 1, player.y);
        cargo_next_pos = Pt(player.x + 2, player.y);
    } else {
        assert(0);
        return 0;
    }

    Tile at_player_next_pos = get_tile(level, player_next_pos);
    int move_player = 0;
    // player move only
    if (at_player_next_pos == Empty || at_player_next_pos == Goal) {
        move_player = 1;
    } else if (at_player_next_pos == Cargo || at_player_next_pos == GoalCargo) {
        Tile at_cargo_next_pos = get_tile(level, cargo_next_pos);

        if (at_cargo_next_pos == Empty) {
            move_player = 1;

            level->board[cargo_next_pos.x][cargo_next_pos.y] = Cargo;

        } else if (at_cargo_next_pos == Goal) {
            move_player = 1;

            level->board[cargo_next_pos.x][cargo_next_pos.y] = GoalCargo;
        }
    }

    if (move_player) {
        if (at_player_next_pos == Cargo) {
            level->board[player_next_pos.x][player_next_pos.y] = Empty;
        } else if (at_player_next_pos == GoalCargo) {
            level->board[player_next_pos.x][player_next_pos.y] = Goal;
        }
        level->playerPos = player_next_pos;
    }

    return move_player;
}
// Note(nos): This code assumes the level is all surrounded by walls.
// otherwise we do out of bounds access. Levels *should* have walls all around,
// but we don't verify this anywhere.

int move(Level *level, enum Direction d)
{
    Point g = level->playerPos;
    int moved = 0;

    /* this is messy; no time for math */
    switch (d) {
        case UP:
            switch (get_tile(level, Pt(g.x, g.y - 1))) {
                case Empty:
                case Goal:
                    moved = 1;
                    level->playerPos = Pt(g.x, g.y - 1);
                    break;
                case Cargo:
                case GoalCargo:
                    switch (get_tile(level, Pt(g.x, g.y - 2))) {
                        case Empty:
                            moved = 1;
                            level->board[g.x][g.y - 2] = Cargo;
                            break;
                        case Goal:
                            moved = 1;
                            level->board[g.x][g.y - 2] = GoalCargo;
                            break;
                    }
                    if (moved) {
                        level->board[g.x][g.y - 1] = (level->board[g.x][g.y - 1] == Cargo) ? Empty : Goal;
                        level->playerPos = Pt(g.x, g.y - 1);
                    }
                    break;
                default:
                    break;
            }
            break;
        case DOWN:
            switch (get_tile(level, Pt(g.x, g.y + 1))) {
                case Empty:
                case Goal:
                    moved = 1;
                    level->playerPos = Pt(g.x, g.y + 1);
                    break;
                case Cargo:
                case GoalCargo:
                    switch (get_tile(level, Pt(g.x, g.y + 2))) {
                        case Empty:
                            moved = 1;
                            level->board[g.x][g.y + 2] = Cargo;
                            break;
                        case Goal:
                            moved = 1;
                            level->board[g.x][g.y + 2] = GoalCargo;
                            break;
                    }
                    if (moved) {
                        level->board[g.x][g.y + 1] = (level->board[g.x][g.y + 1] == Cargo) ? Empty : Goal;
                        level->playerPos = Pt(g.x, g.y + 1);
                    }
                    break;
            }
            break;
        case LEFT:
            switch (get_tile(level, Pt(g.x - 1, g.y))) {
                case Empty:
                case Goal:
                    moved = 1;
                    level->playerPos = Pt(g.x - 1, g.y);
                    break;
                case Cargo:
                case GoalCargo:
                    switch (get_tile(level, Pt(g.x - 2, g.y))) {
                        case Empty:
                            moved = 1;
                            level->board[g.x - 2][g.y] = Cargo;
                            break;
                        case Goal:
                            moved = 1;
                            level->board[g.x - 2][g.y] = GoalCargo;
                            break;
                    }
                    if (moved) {
                        level->board[g.x - 1][g.y] = (level->board[g.x - 1][g.y] == Cargo) ? Empty : Goal;
                        level->playerPos = Pt(g.x - 1, g.y);
                    }
                    break;
            }
            break;
        case RIGHT:
            switch (get_tile(level, Pt(g.x + 1, g.y))) {
                case Empty:
                case Goal:
                    moved = 1;
                    level->playerPos = Pt(g.x + 1, g.y);
                    break;
                case Cargo:
                case GoalCargo:
                    switch (get_tile(level, Pt(g.x + 2, g.y))) {
                        case Empty:
                            moved = 1;
                            level->board[g.x + 2][g.y] = Cargo;
                            break;
                        case Goal:
                            moved = 1;
                            level->board[g.x + 2][g.y] = GoalCargo;
                            break;
                    }
                    if (moved) {
                        level->board[g.x + 1][g.y] = (level->board[g.x + 1][g.y] == Cargo) ? Empty : Goal;
                        level->playerPos = Pt(g.x + 1, g.y);
                    }
                    break;
            }
            break;
    }

    return moved;
}

static void init_level(Level *l)
{
    int x, y;
    l->width = l->height = 0;
    l->lvlnr = 0;
    l->playerPos.x = l->playerPos.y = 0;
    for (x = 0; x < MAX_X; x++)
        for (y = 0; y < MAX_Y; y++) l->board[x][y] = Empty;
}

#if 0
static void mark_background(Level * l)
{
	int x, y;
	for (y = 0; y < l->height; y++) {
		int sx = 0, bx = l->width;
		while (sx < l->width && l->board[sx][y] == Empty)
			sx++;
		while (bx > 0 && l->board[bx][y] == Empty)
			bx--;
		for (; sx < bx; sx++) {
			if (l->board[sx][y] == Empty)
				l->board[sx][y] = Background;
			else if (l->board[sx][y] == Cargo)
				l->board[sx][y] = Cargo;


		}
	}
	for (x = 0; x < l->width; x++) {
		int sy = 0, by = l->height;
		while (sy < l->height && l->board[x][sy] != Wall)
			l->board[x][sy++] = Empty;
		while (by >= 0 && l->board[x][by] != Wall)
			l->board[x][by--] = Empty;
	}
}
#endif

int load_levels(GameData *game, const char *filename)
{
    FILE *f;
    LevelSet *set = &game->levels;
    Level *l;
    int lnum = 0;
    int x = 0, y = 0;
    int c;

    game->currentLevel = nulllevel;

    f = fopen(filename, "r");
    if (f == NULL) return -1;

    set->name = larena_alloc(game->arena, strlen(filename) + 1);
    strcpy(set->name, filename);

    set->nrlevels = 0;
    set->levels = LARENA_ALLOC_TYPE(game->arena, Level);
    if (set->levels == NULL) {
        fclose(f);
        return -1;
    }
    l = &set->levels[0];
    init_level(l);

    while ((c = fgetc(f)) != EOF) {
        switch (c) {
            case ';':
                next_line(f);
                break;
            case ' ':
                l->board[x][y] = Empty;
                x++;
                break;
            case '.':
                l->board[x][y] = Goal;
                x++;
                break;
            case '$':
                l->board[x][y] = Cargo;
                x++;
                break;
            case '@':

                l->board[x][y] = Empty;
                l->playerPos.x = x;
                l->playerPos.y = y;
                x++;
                break;
            case '+':
                l->board[x][y] = Goal;
                l->playerPos.x = x;
                l->playerPos.y = y;
                x++;
                break;
            case '#':
                l->board[x][y] = Wall;
                x++;
                break;
            case '*':
                l->board[x][y] = GoalCargo;
                x++;
                break;
            case '\n':
                ++y;
                x = 0;
                c = fgetc(f);
                if (c == '\n' || c == EOF) {
                    lnum++;
                    l->lvlnr = lnum;
                    set->nrlevels++;
                    // We're only allocating Level in this loop, so we know it's
                    // laid out continously at set->levels
                    Level *nl = LARENA_ALLOC_TYPE(game->arena, Level);
                    if (nl == NULL) {
                        fprintf(stdout, "Out of memory loading levels\n");
                        abort();
                    }

                    l = nl;
                    init_level(l);
                    y = 0;
                } else
                    ungetc(c, f);
                break;

            default:
                fprintf(stderr, "Unexpected '%c' in %s\n", c, filename);
                break;
        }
        if (l->width < x) l->width = x;
        l->height = y + 1;
        if (x >= MAX_X || y >= MAX_Y) {
            fprintf(stderr, "To big board '%d,%d' in %s\n", x, y, filename);
            x = MIN(x - 1, MAX_X - 1);
            y = MIN(y - 1, MAX_Y - 1);
        }
    }

    fclose(f);

    return 0;
}

int set_level(GameData *game, int nr)
{
    Level *current = &game->currentLevel;
    LevelSet *set = &game->levels;

    if (nr < set->nrlevels && nr >= 0) {
        *current = set->levels[nr];
        game->nr_moves = 0;
        return 0;
    }

    return -1;
}

int next_level(GameData *game)
{
    Level *current = &game->currentLevel;
    LevelSet *set = &game->levels;
    *current = set->levels[(current->lvlnr) % set->nrlevels];
    game->nr_moves = 0;

    return 0;
}

int restart_level(GameData *game)
{
    Level *current = &game->currentLevel;
    LevelSet *set = &game->levels;
    *current = set->levels[(current->lvlnr - 1) % set->nrlevels];
    game->nr_moves = 0;

    return 0;
}

void init_game(GameData *game, LArena *arena)
{
    memset(game, 0, sizeof *game);
    game->currentLevel = nulllevel;
    game->arena = arena;
}

void print_levels(LevelSet *l)
{
    int i;

    for (i = 0; i < l->nrlevels; i++) {
        Level *v;
        int x, y;
        v = &l->levels[i];
        printf("Level %d, v=%d,h=%d\n", i + 1, v->width, v->height);
        for (y = 0; y < v->height; y++) {
            for (x = 0; x < v->width; x++) {
                switch (v->board[x][y]) {
                    case Empty:
                        putchar(' ');
                        break;
                    case Background:
                        putchar(' ');
                        break;
                    case Wall:
                        putchar('#');
                        break;
                    case Cargo:
                        putchar('$');
                        break;
                    case Goal:
                        putchar('.');
                        break;
                    case GoalCargo:
                        putchar('*');
                        break;
                }
            }
            putchar('\n');
        }
        putchar('\n');
        putchar('\n');
    }
}

#if 0
int main(int argc, char *argv[])
{
	LevelSet *L;
	if (argc != 2)
		return 1;

	L = load_levels(argv[1]);
	print_levels(L);

	return 0;
}
#endif
// Made by Nils O. Selåsdal
