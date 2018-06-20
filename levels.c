#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <GL/gl.h>
#include "sokoban.h"

#define MIN(a, b)  (((a) < (b)) ? (a) : (b))
Level currentlevel;
static Level nulllevel;
static LevelSet *currentset;

int check_goal(void)
{
	int x, y;

	for (y = 0; y < currentlevel.height; y++) {
		for (x = 0; x < currentlevel.width; x++) {

			if (currentlevel.board[x][y] == Goal)
				return 0;
		}
	}

	return 1;
}
static void next_line(FILE * f)
{
	int c;
	while ((c = fgetc(f)) != EOF && c != '\n');
}
static inline Point Pt(int x, int y)
{
	Point p = { x, y };
	return p;
}

//the move routine taken from the Plan9 sokoban game. Thanks :) /me lazy
int move(enum Direction d)
{
	Point g = currentlevel.pos;
	int moved = 0;

	/* this is messy; no time for math */
	switch (d) {
	case UP:
		switch (currentlevel.board[g.x][g.y - 1]) {
		case Empty:
		case Goal:
			moved = 1;
			currentlevel.pos = Pt(g.x, g.y - 1);
			break;
		case Cargo:
		case GoalCargo:
			switch (currentlevel.board[g.x][g.y - 2]) {
			case Empty:
				moved = 1;
				currentlevel.board[g.x][g.y - 2] = Cargo;
				break;
			case Goal:
				moved = 1;
				currentlevel.board[g.x][g.y - 2] = GoalCargo;
				break;
			}
			if (moved) {
				currentlevel.board[g.x][g.y - 1] = (currentlevel.board[g.x][g.y - 1] == Cargo) ? Empty : Goal;
				currentlevel.pos = Pt(g.x, g.y - 1);
			}
			break;
		default:
			break;
		}
		break;
	case DOWN:
		switch (currentlevel.board[g.x][g.y + 1]) {
		case Empty:
		case Goal:
			moved = 1;
			currentlevel.pos = Pt(g.x, g.y + 1);
			break;
		case Cargo:
		case GoalCargo:
			switch (currentlevel.board[g.x][g.y + 2]) {
			case Empty:
				moved = 1;
				currentlevel.board[g.x][g.y + 2] = Cargo;
				break;
			case Goal:
				moved = 1;
				currentlevel.board[g.x][g.y + 2] = GoalCargo;
				break;
			}
			if (moved) {
				currentlevel.board[g.x][g.y + 1] = (currentlevel.board[g.x][g.y + 1] == Cargo) ? Empty : Goal;
				currentlevel.pos = Pt(g.x, g.y + 1);
			}
			break;
		}
		break;
	case LEFT:
		switch (currentlevel.board[g.x - 1][g.y]) {
		case Empty:
		case Goal:
			moved = 1;
			currentlevel.pos = Pt(g.x - 1, g.y);
			break;
		case Cargo:
		case GoalCargo:
			switch (currentlevel.board[g.x - 2][g.y]) {
			case Empty:
				moved = 1;
				currentlevel.board[g.x - 2][g.y] = Cargo;
				break;
			case Goal:
				moved = 1;
				currentlevel.board[g.x - 2][g.y] = GoalCargo;
				break;
			}
			if (moved) {
				currentlevel.board[g.x - 1][g.y] = (currentlevel.board[g.x - 1][g.y] == Cargo) ? Empty : Goal;
				currentlevel.pos = Pt(g.x - 1, g.y);
			}
			break;
		}
		break;
	case RIGHT:
		switch (currentlevel.board[g.x + 1][g.y]) {
		case Empty:
		case Goal:
			moved = 1;
			currentlevel.pos = Pt(g.x + 1, g.y);
			break;
		case Cargo:
		case GoalCargo:
			switch (currentlevel.board[g.x + 2][g.y]) {
			case Empty:
				moved = 1;
				currentlevel.board[g.x + 2][g.y] = Cargo;
				break;
			case Goal:
				moved = 1;
				currentlevel.board[g.x + 2][g.y] = GoalCargo;
				break;
			}
			if (moved) {
				currentlevel.board[g.x + 1][g.y] = (currentlevel.board[g.x + 1][g.y] == Cargo) ? Empty : Goal;
				currentlevel.pos = Pt(g.x + 1, g.y);
			}
			break;
		}
		break;
	}

	return moved;
}

static Level *new_level(void)
{
	Level *l = malloc(sizeof(*l));
	int x, y;
	if (l == NULL)
		return NULL;
	l->width = l->height = -1;
	l->pos.x = l->pos.y = -1;
	l->lvlnr = -1;
	for (x = 0; x < MAX_X; x++)
		for (y = 0; y < MAX_Y; y++)
			l->board[x][y] = Empty;

	return l;
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

Level *current_level(void)
{
	return &currentlevel;
}
int load_levels(const char *filename)
{
	FILE *f;
	LevelSet *set;
	Level *l;
	int lnum = 0;
	int x = 0, y = 0;
	int c;

	currentlevel = nulllevel;


	f = fopen(filename, "r");
	if (f == NULL)
		return -1;

	set = malloc(sizeof(*set));
	if (set == NULL) {
  		fclose(f);
		return -1;
	}
	l = new_level();
	if (l == NULL) {
		free(set);
 		fclose(f);
		return -1;
	}
	set->name = strdup(filename);
	set->nrlevels = 0;
	set->levels = NULL;

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
			l->pos.x = x;
			l->pos.y = y;
			x++;
			break;
		case '+':
			l->board[x][y] = Goal;
			l->pos.x = x;
			l->pos.y = y;
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
				Level **nl;
				nl = realloc(set->levels, sizeof(Level *) * (lnum + 1));
				if (nl == NULL) {
					fprintf(stdout, "Out of memory loading levels\n");
					abort();
				}

				set->levels = nl;

				set->levels[lnum] = l;
				set->nrlevels++;
				lnum++;
				l->lvlnr = lnum;
				l = new_level();
				y = 0;
			} else
				ungetc(c, f);
			break;

		default:
			fprintf(stderr, "Unexpected '%c' in %s\n", c, filename);
			break;

		}
		if (l->width < x)
			l->width = x;
		l->height = y + 1;
		if (x >= MAX_X || y >= MAX_Y) {
			fprintf(stderr, "To big board '%d,%d' in %s\n", x, y, filename);
			x = MIN(x - 1, MAX_X - 1);
			y = MIN(x - 1, MAX_Y - 1);

		}

	}

	fclose(f);

	currentset = set;
	return 0;
}

int set_level(int nr)
{
	if (nr < currentset->nrlevels && nr >= 0) {
		currentlevel = *currentset->levels[nr];
		return 0;
	}

	return -1;
}

int next_level(void)
{
	currentlevel = *currentset->levels[(currentlevel.lvlnr) % currentset->nrlevels];
	return 0;
}

int restart_level(void)
{
	currentlevel = *currentset->levels[(currentlevel.lvlnr - 1) % currentset->nrlevels];
	return 0;
}

void print_levels(LevelSet * l)
{
	int i;

	for (i = 0; i < l->nrlevels; i++) {
		Level *v;
		int x, y;
		v = l->levels[i];
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
//Made by Nils O. Selåsdal
