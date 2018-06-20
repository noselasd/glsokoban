
#include <SDL/SDL.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include "common.h"
#include "camera.h"

Camera mainCamera;

SDL_Surface *mainWindow;
void set_view(int w, int h)
{
	float ratio = (float) w / (float) h;

	glMatrixMode(GL_PROJECTION);

	/*
	 * Change to the projection matrix and set
	 * our viewing volume.
	 */
	glLoadIdentity();
	glViewport(0, 0, w, h);
	gluPerspective(FOV, ratio, NEAR_PLANE, FAR_PLANE);
	//glFrustum(-1.333333 ,1.333333 , -1.000000 , 1.000000, 1.000000, 500.000000);

	glMatrixMode(GL_MODELVIEW);
	//load identity matrix for modelview, so we initialize the camera at 0,0,0
	glLoadIdentity();

}

static void setup_opengl(int width, int height)
{
	GLfloat ambient[] = { 0.5, 0.5, 0.5, 1.0 };
	GLfloat diffuse[] = { 0.7, 0.7, 0.7, 1.0 };
	GLfloat specular[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat position[] = { 1000.0, 1000.0, 1000.0, 0.0 };
	GLfloat lmodel_ambient[] = { 0.3, 0.3, 0.3, 1.0 };
	GLfloat local_view[] = { 0.0 };



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
	//glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
	// glLightModelfv(GL_LIGHT_MODEL_LOCAL_VIEWER, local_view);

}

void init(void)
{
	/* Information about the current video settings. */
	const SDL_VideoInfo *info = NULL;
	/* Dimensions of our window. */
	int width;
	int height;
	/* Color depth in bits of our window. */
	int bpp = 0;
	/* Flags we will pass into SDL_SetVideoMode. */
	int flags = 0;

	/* First, initialize SDL's video subsystem. */
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		/* Failed, exit. */
		fprintf(stderr, "Video initialization failed: %s\n", SDL_GetError());
		do_quit(1);
	}
    //enable vsync
    int ret = SDL_GL_SetAttribute( SDL_GL_SWAP_CONTROL, 1 );
    if(ret != 0) 
		fprintf(stderr, "Enabling vsync failed: %s\n", SDL_GetError());


    
	/* Let's get some video information. */
	info = SDL_GetVideoInfo();

	if (!info) {
		/* This should probably never happen. */
		fprintf(stderr, "Video query failed: %s\n", SDL_GetError());
		do_quit(1);
	}

	width = WIDTH;
	height = HEIGHT;
	bpp = info->vfmt->BitsPerPixel;

	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	flags = SDL_OPENGL;	//;| SDL_HWPALETTE;
	if (info->hw_available) {	// is hardware blitting available
		flags |= SDL_HWSURFACE;
		puts("Hardware available");
	}
	if (info->blit_hw) {
		flags |= SDL_HWACCEL;
		puts("Hardware blit available");
	}

	/*
	 * Set the video mode
	 */
	mainWindow = SDL_SetVideoMode(width, height, bpp, flags);
	if (mainWindow == NULL) {
		/* 
		 * This could happen for a variety of reasons,
		 * including DISPLAY not being set, the specified
		 * resolution not being available, etc.
		 */
		fprintf(stderr, "Video mode set failed: %s\n", SDL_GetError());
		do_quit(1);
	}

	SDL_WM_SetCaption("glSokoban", "glSokoban");	// set the window caption (first argument) and icon caption (2nd arg)
	/*
	 * At this point, we should have a properly setup
	 * double-buffered window for use with OpenGL.
	 */
	setup_opengl(width, height);

}

//Made by Nils O. Selåsdal
