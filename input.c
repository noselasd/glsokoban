#include <SDL.h>
#if defined(__APPLE__)
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif

#include "common.h"
#include "input.h"
#include "init.h"
#include "sokoban.h"

static GLboolean keys[SDLK_ENDCALL];
GLboolean isFullScreen = GL_FALSE;

static enum Direction currmove;

enum Direction get_move(void)
{
    return currmove;
}

static void ToggleFullScreen(void)
{
    int flags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE;
    flags |= isFullScreen ? 0 : SDL_WINDOW_FULLSCREEN;
    if (SDL_SetWindowFullscreen(mainWindow, flags) != 0)  // try to toggle fullscreen mode for window 'main_window'
    {
        printf("Failed to Toggle Fullscreen mode : %s\n", SDL_GetError());  // report error in case toggle fails
    }
    if (!isFullScreen) {
        isFullScreen = GL_TRUE;
        SDL_ShowCursor(SDL_DISABLE);
    } else {
        isFullScreen = GL_FALSE;
        SDL_ShowCursor(SDL_ENABLE);
    }
    DEBUG("Fullscreen is %d\n", isFullScreen);
}

GLboolean key_state(SDL_KeyCode keycode)
{
    return keys[keycode];
}

static void handle_key_up(SDL_Keysym *keysym)
{
    keys[keysym->sym] = GL_FALSE;
    switch (keysym->sym) {
        case SDLK_F2: {
            static int f2 = 0;
            GLfloat local_view[1];

            local_view[0] = f2 ? 1.0 : 0.0;
            glLightModelfv(GL_LIGHT_MODEL_LOCAL_VIEWER, local_view);
            f2 = 1 - f2;
        } break;
        case SDLK_F3: {
            static int f3 = 0;
            f3 ? glEnable(GL_LIGHTING) : glDisable(GL_LIGHTING);
            f3 = 1 - f3;
        } break;
        case SDLK_F4: {
            static int f4 = 0;
            f4 ? glCullFace(GL_BACK) : glCullFace(GL_FRONT);
            f4 = 1 - f4;
        } break;
        case SDLK_F5: {
            static int f5 = 0;
            f5 ? glShadeModel(GL_SMOOTH) : glShadeModel(GL_FLAT);
            f5 = 1 - f5;
        } break;
        default:
            break;
    };
}

static enum K_Command handle_key_down(SDL_Keysym *keysym)
{
    keys[keysym->sym] = GL_TRUE;
    enum K_Command cmd = NOTHING;
    switch (keysym->sym) {
        case SDLK_ESCAPE:
            do_quit(0);
            break;
        case SDLK_F1:            // if it is F1
            ToggleFullScreen();  // toggle between fullscreen and windowed mode
            break;
        case SDLK_LEFT:
            cmd = MOVE;
            currmove = LEFT;
            break;
        case SDLK_UP:
            cmd = MOVE;
            currmove = UP;
            break;
        case SDLK_DOWN:
            cmd = MOVE;
            currmove = DOWN;
            break;
        case SDLK_RIGHT:
            cmd = MOVE;
            currmove = RIGHT;
            break;
        case SDLK_r:
            cmd = RESTART_LVL;
            break;
        case SDLK_n:
            cmd = NEXT_LVL;
            break;
        default:
            break;
    }

    return cmd;
}

enum K_Command process_events(void)
{
    /* Our SDL event placeholder. */
    SDL_Event event;
    enum K_Command cmd = NOTHING;
    /* Grab all the events off the queue. */
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_KEYDOWN:
                /* Handle key presses. */
                cmd = handle_key_down(&event.key.keysym);
                break;
            case SDL_KEYUP:
                /* Handle key presses. */
                handle_key_up(&event.key.keysym);
                break;
            case SDL_QUIT:
                DEBUG("Got quit event%s", "\n");
                /* Handle quit requests (like Ctrl-c). */
                do_quit(0);
                break;
            case SDL_WINDOWEVENT:
                switch (event.window.event) {
                    case SDL_WINDOWEVENT_RESIZED:
                    case SDL_WINDOWEVENT_SIZE_CHANGED:
                        cmd = WINDOW_RESIZED;
                        break;
                }
                break;
        }
    }

    return cmd;
}

// Made by Nils O. Selåsdal
