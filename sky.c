#include <GL/gl.h>
#include <GL/glu.h>
#include <SDL/SDL.h>
#include <math.h>
#include <stdlib.h>
#include "timer.h"
#include "common.h"
#include "sokoban.h"

static GLuint back_list;

#define BACK_DIST 100.0f

void back_init(void)
{
	GLUquadric *back_quad = NULL;
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	if ((back_quad = gluNewQuadric())) {

		gluQuadricOrientation(back_quad, GLU_INSIDE);
		gluQuadricNormals(back_quad, GLU_SMOOTH);
		gluQuadricTexture(back_quad, GL_TRUE);

		back_list = glGenLists(1);
		glNewList(back_list, GL_COMPILE);
		glScalef(BACK_DIST, BACK_DIST, BACK_DIST);
		glRotatef(120.0f, 1.0f, 0.0f, 0.0f);
		glColor3f(1.0f, 1.0f, 1.0f);
		glBindTexture(GL_TEXTURE_2D, textures[Sky]);
		gluSphere(back_quad, 1.0f, 64, 32);
		glEndList();
	} else
		abort();

	gluDeleteQuadric(back_quad);
}

void back_draw(void)
{
	float f = 6.0f * sinf(timer_since_started(&frameTimer) * 0.1f);

	glPushAttrib(GL_LIGHTING_BIT | GL_DEPTH_BITS);
	glPushMatrix();
	glDisable(GL_LIGHTING);
	glDepthMask(GL_FALSE);

	glRotatef(f, 0.0f, 1.0f, 0.0f);
	glCallList(back_list);

	glPopMatrix();
	glPopAttrib();
}

//Made by Nils O. Selåsdal
