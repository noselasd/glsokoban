#include <SDL/SDL.h>
#include <GL/gl.h>
#include <GL/glext.h>
#include <math.h>
#include <GL/glu.h>
#include <SDL/SDL_image.h>
#include "common.h"
#include "material.h"
#include "sokoban.h"



GLuint textures[NumTextures];

static void load_texture(const char *filename, GLint textid)
{
	SDL_Surface *texture;

	texture = IMG_Load(filename);
	if (texture == NULL) {
		fprintf(stderr, "Failed to load %s\n", filename);
		return;
	}
	SDL_LockSurface(texture);	//Remember to always lock the surface before using the pixel data

	glBindTexture(GL_TEXTURE_2D, textid);

	/* Generate The Texture */
	if (texture->format->BytesPerPixel == 3)
		glTexImage2D(GL_TEXTURE_2D, 0, 3, texture->w, texture->h, 0, GL_BGR, GL_UNSIGNED_BYTE, texture->pixels);
	else if (texture->format->BytesPerPixel == 4)
		glTexImage2D(GL_TEXTURE_2D, 0, 4, texture->w, texture->h, 0, GL_BGRA, GL_UNSIGNED_BYTE, texture->pixels);


	/* Nearest Filtering */
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	/* Linear Filtering */
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	SDL_UnlockSurface(texture);

	SDL_FreeSurface(texture);
}

void load_textures(void)
{

	/* Create The Texture */
	glGenTextures(NumTextures, textures);
	load_texture(MAINDIR "/textures/crate.bmp", textures[Brick]);
	load_texture(MAINDIR "/textures/horiz_one.bmp", textures[Sky]);


}

//Made by Nils O. Selåsdal
