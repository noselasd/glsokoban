#ifndef __INPUT_H__
#define __INPUT_H__
#include <SDL_keycode.h>
#if defined(__APPLE__)
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

enum K_Command { NOTHING, MOVE, RESTART_LVL, NEXT_LVL, WINDOW_RESIZED };

extern GLboolean isFullScreen;
enum Direction get_move(void);
GLboolean key_state(SDL_KeyCode keycode);
enum K_Command process_events(void);
#endif
