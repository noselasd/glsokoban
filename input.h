#ifndef __INPUT_H__
#define __INPUT_H__
#include <SDL/SDL_keysym.h>
#include <GL/gl.h>

enum K_Command {
	NOTHING,
	MOVE,
	RESTART_LVL,
	NEXT_LVL
};

extern GLboolean isFullScreen;
enum Direction get_move(void);
GLboolean key_state(SDLKey keycode);
enum K_Command process_events(void);
#endif
