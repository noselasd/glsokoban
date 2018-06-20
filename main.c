#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include "common.h"
#include "maincamera.h"
#include "init.h"
#include "input.h"
#include "timer.h"
#include "sokoban.h"

Timer frameTimer;

void do_quit(int code)
{
	exit(code);
}

static void install_signals()
{


	signal(SIGTERM, do_quit);
	signal(SIGINT, do_quit);	//ctrl+c
#ifdef __unix
	signal(SIGHUP, do_quit);
	signal(SIGQUIT, do_quit);
	signal(SIGSEGV, do_quit);	//segmentation failt

#endif
}

int main(int argc, char *argv[])
{
	int moves = 0;
	/* Information about the current video settings. */
	double fpslast = 0.0f;
	int ok;
	int frames = 0;
	const char *levelfile;
	install_signals();
	init();
	init_fonts();
	timer_init(&frameTimer);
	atexit(SDL_Quit);

	if (argc > 1) {
		levelfile = argv[1];
	} else
		levelfile = MAINDIR "/levels/easy.slc";

	if (load_levels(levelfile) != 0) {
		fprintf(stderr, "Could not load level %s\n", levelfile);
		exit(3);
	}
	set_level(0);
	load_textures();
	srand((unsigned int) SDL_GetTicks());
	frame_init();
	frame_newlevel();

	/* main loop */
	for (;;) {
		enum K_Command cmd;
		int moved = 0, finished = 0;
		timer_update(&frameTimer);
		/* Process incoming events. */
		cmd = process_events();
		switch (cmd) {
		case MOVE:
			moved = move(get_move());
			break;
		case RESTART_LVL:
			restart_level();
			frame_newlevel();
			moves = 0;
			break;
		case NEXT_LVL:
			next_level();
			frame_newlevel();
			moves = 0;
			break;
		case NOTHING:
		default:
			break;
		}

		if (moved) {
			++moves;
			finished = check_goal();
		}
		/* Draw the screen. */
		if (finished) {
			Timer finisht;
			timer_init(&finisht);
			do {

				frame_begin(moves, finished);
				frames++;
				fpslast += timer_last_delta(&frameTimer);
				timer_update(&frameTimer);

			} while (timer_since_started(&finisht) <= 3.1415f);	//for 3.1415 seconds, stall and draw finished
			next_level();
			moves = 0;
			frame_newlevel();
		} else {
			frame_begin(moves, finished);
			frames++;
			fpslast += timer_last_delta(&frameTimer);
		}

		if (fpslast >= 4.0f) {
			DEBUG("fps %f\n", frames / 4.0f);
			fpslast = 0.0f;
			frames = 0;
		}
	}

	/* Never reached. */
	return 0;
}

//Made by Nils O. Selåsdal
