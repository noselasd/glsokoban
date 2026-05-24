
#include <SDL.h>
#if defined(__APPLE__)
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif

#include "common.h"

SDL_Window *mainWindow;
void set_view(int w, int h)
{
    float ratio = (float)w / (float)h;

    glMatrixMode(GL_PROJECTION);

    /*
     * Change to the projection matrix and set
     * our viewing volume.
     */
    glLoadIdentity();
    glViewport(0, 0, w, h);
    gluPerspective(FOV, ratio, NEAR_PLANE, FAR_PLANE);
    // glFrustum(-1.333333 ,1.333333 , -1.000000 , 1.000000, 1.000000, 500.000000);

    glMatrixMode(GL_MODELVIEW);
    // load identity matrix for modelview, so we initialize the camera at 0,0,0
    glLoadIdentity();
}

static void setup_opengl(int width, int height)
{
    GLfloat ambient[] = {0.5, 0.5, 0.5, 1.0};
    GLfloat diffuse[] = {0.7, 0.7, 0.7, 1.0};
    GLfloat specular[] = {1.0, 1.0, 1.0, 1.0};
    GLfloat position[] = {1000.0, 1000.0, 1000.0, 0.0};
    GLfloat lmodel_ambient[] = {0.3, 0.3, 0.3, 1.0};
    GLfloat local_view[] = {0.0};

    set_view(width, height);

    /* Our shading model--Gouraud (smooth). */
    glShadeModel(GL_SMOOTH);

    /* Culling. */
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_TEXTURE_2D);

    /* Set the clear color. */
    glClearColor(0.0, 0.0, 0.0, 0);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
    // glLightfv(GL_LIGHT0, GL_POSITION, position);
    // glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
    // glLightModelfv(GL_LIGHT_MODEL_LOCAL_VIEWER, local_view);
    SDL_GL_SetSwapInterval(1);  // enable vsync
}

void init(void)
{
    /* Dimensions of our window. */
    int width;
    int height;
    /* Flags we will pass into SDL_SetVideoMode. */
    int flags = 0;

    // enable vsync

    width = WIDTH;
    height = HEIGHT;

    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    flags = SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL;  //;| SDL_HWPALETTE;

    /*
     * Set the video mode
     */
    mainWindow = SDL_CreateWindow("glSokoban", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, flags);
    // mainWindow = SDL_SetVideoMode(width, height, bpp, flags);
    if (mainWindow == NULL) {
        /*
         * This could happen for a variety of reasons,
         * including DISPLAY not being set, the specified
         * resolution not being available, etc.
         */
        fprintf(stderr, "SDL_CreateWindow failed: %s\n", SDL_GetError());
        do_quit(1);
    }
    if (SDL_GL_CreateContext(mainWindow) == NULL) {
        fprintf(stderr, "SDL_GL_CreateContext failed: %s\n", SDL_GetError());
        do_quit(2);
    }
    /*
     * At this point, we should have a properly setup
     * double-buffered window for use with OpenGL.
     */
    setup_opengl(width, height);
    SDL_RaiseWindow(mainWindow);
}

// Made by Nils O. Selåsdal
