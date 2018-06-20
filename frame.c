#include <SDL/SDL.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <stdlib.h>
#include <math.h>
#include "vector.h"
#include "common.h"
#include "sky.h"
#include "maincamera.h"
#include "material.h"
#include "sokoban.h"

static GLUquadric *sphereQ;
static const char *finish_text = NULL;

static void start2d(void);
static void stop2d(void);
void calc_distance(void);

static void draw_cube(int x, int y)
{
	glPushMatrix();
	glTranslatef((float) x + 0.5f, (float) -y + 0.5f, 0.0f);
	double size = 0.5f;

#   define V(a,b,c) glVertex3d( a size, b size, c size )
#   define N(a,b,c) glNormal3d( a, b, c )
#   define T1 glTexCoord2i( 1,  1 )
#   define T2 glTexCoord2i( 1,  0 )
#   define T3 glTexCoord2i( 0,  0 )
#   define T4 glTexCoord2i( 0,  1 )

	glBegin(GL_QUADS);
	N(0.0f, 0.0f, -1.0f);
	T1;
	V(-, -, -);
	T2;
	V(-, +, -);
	T3;
	V(+, +, -);
	T4;
	V(+, -, -);
	N(0.0f, 0.0f, 1.0f);
	T1;
	V(+, +, +);
	T2;
	V(-, +, +);
	T3;
	V(-, -, +);
	T4;
	V(+, -, +);
	N(0.0f, 1.0f, 0.0f);
	T1;
	V(+, +, +);
	T2;
	V(+, +, -);
	T3;
	V(-, +, -);
	T4;
	V(-, +, +);
	N(0.0f, -1.0f, 0.0f);
	T1;
	V(-, -, +);
	T2;
	V(-, -, -);
	T3;
	V(+, -, -);
	T4;
	V(+, -, +);
	N(-1.0f, 0.0f, 0.0f);
	T1;
	V(-, -, +);
	T2;
	V(-, +, +);
	T3;
	V(-, +, -);
	T4;
	V(-, -, -);
	N(1.0f, 0.0f, 0.0f);
	T1;
	V(+, -, +);
	T2;
	V(+, -, -);
	T3;
	V(+, +, -);
	T4;
	V(+, +, +);
	glEnd();

#   undef V
#   undef N
#   undef T1
#   undef T2
#   undef T3
#   undef T4

	glPopMatrix();
}

static void draw_wall(int x, int y)
{
	material_apply(&m_normal);
	glBindTexture(GL_TEXTURE_2D, textures[Brick]);
	draw_cube(x, y);
	glBindTexture(GL_TEXTURE_2D, 0);

	return;
}

static void draw_cargo(int x, int y)
{
	material_apply(&m_ruby);
	glTranslatef((float) x + 0.5f, (float) -y + 0.5f, 0.0f);
	gluSphere(sphereQ, 0.48f, 29, 29);
	glTranslatef((float) -x - 0.5f, (float) y - 0.5f, 0.0f);	//and translate back again


}
static void draw_alien(Point * p)
{
	material_apply(&m_silver);
	glTranslatef((float) p->x + 0.5f, (float) -p->y + 0.5f, 0.0f);
	gluSphere(sphereQ, 0.48f, 29, 29);
	glTranslatef((float) -p->x - 0.5f, (float) p->y - 0.5f, 0.0f);	//and translate back again
}

static void draw_background(int x, int y)
{

	material_apply(&m_silver);
	glBegin(GL_QUADS);
	glNormal3f(0.0f, 0.0f, 1.0f);
	glVertex3f(x, -y, -0.5f);
	glVertex3f(x + 1, -y, -0.5f);
	glVertex3f(x + 1, -y + 1, -0.5f);
	glVertex3f(x, -y + 1, -0.5f);
	glEnd();
}

static void draw_goal(int x, int y)
{
	glBegin(GL_QUADS);
	glNormal3f(0.0f, 0.0f, 1.0f);
	glVertex3f(x, -y, -0.5f);
	glVertex3f(x + 1, -y, -0.5f);
	glVertex3f(x + 1, -y + 1, -0.5f);
	glVertex3f(x, -y + 1, -0.5f);
	glEnd();
}

static void draw_goalcargo(int x, int y)
{
	material_apply(&m_lightblue);
	draw_goal(x, y);
	draw_cargo(x, y);
}

static void glDrawLevel(Level * l)
{
	int x, y;

	for (y = 0; y < l->height; y++) {
		for (x = 0; x < l->width; x++) {
			switch (l->board[x][y]) {

			case Empty:
				continue;
			case Background:	//Not used..
				draw_background(x, y);
				break;
			case Wall:
				draw_wall(x, y);
				break;
			case Cargo:
				draw_cargo(x, y);
				break;
			case Goal:
				material_apply(&m_blue);
				draw_goal(x, y);
				break;
			case GoalCargo:
				draw_goalcargo(x, y);
				break;
			}

		}
	}

	draw_alien(&l->pos);
}

void frame_init(void)
{
	camera_init(&mainCamera);
	sphereQ = gluNewQuadric();
	gluQuadricOrientation(sphereQ, GLU_OUTSIDE);
	gluQuadricNormals(sphereQ, GLU_SMOOTH);
	back_init();
}

void frame_newlevel(void)
{
	static const char *const finish_texts[] = {
		"Level Finished!",
		"Congratulations!",
		"You made it !!",
		"Well Done !!",
		"Goal! Goal!"
	};
	finish_text = finish_texts[rand() % sizeof(finish_texts) / sizeof *finish_texts];
	camera_init(&mainCamera);
	calc_distance();
}

void frame_begin(int moves, int finished)
{
	float cameraMatrix[16];
	// Material values
	static const GLfloat position[] = { 100.0f, 100.0f, 200.0f, 0.0f };
	char movestr[32];

	move_main_camera();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	quat_getmatrix(&mainCamera.orientation, cameraMatrix);
	glLoadMatrixf(cameraMatrix);
	glLightfv(GL_LIGHT0, GL_POSITION, position);
	glTranslatef(-mainCamera.position.x, -mainCamera.position.y, -mainCamera.position.z);
	back_draw();
	//glTranslatef(0.0f,-10.0f,-10.0f);
	//glRotatef(-90,0.0f,0.0f,1.0f);

	//glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
	//glColorMaterial(GL_FRONT,GL_AMBIENT_AND_DIFFUSE);
	//glEnable(GL_COLOR_MATERIAL);
	glDrawLevel(&currentlevel);
	start2d();
	glColor3f(0.9f, 0.9f, 0.9f);
	ftPrint("glSokoban", 0.01f, 0.01f, 0.028f);
	snprintf(movestr, sizeof(movestr) - 1, "Moves: %d", moves);
	ftPrint(movestr, 0.01, 0.95, 0.028);
	if (finished)
		ftPrint(finish_text, 0.40f, 0.50f, 0.048f);

	stop2d();

	SDL_GL_SwapBuffers();

}

//(try to) calc the apropriate distance and translation to view
//the entire sceme.
void calc_distance(void)
{
	float max;
	float zadjust;
	float fov = FOV;

	fov /= 2.0f;
	fov *= M_PI / 180.0f;	//to radians

	if (currentlevel.height >= currentlevel.width)
		max = currentlevel.height;
	else
		max = currentlevel.width / 1.33333;
	zadjust = fabs((max) / tan(fov));
	camera_truck_y(&mainCamera, -currentlevel.height / 2.6666);
	camera_truck_x(&mainCamera, currentlevel.width / 2.0f);
	camera_truck_z(&mainCamera, (zadjust / 2.0f) + 3.0f);
}

void stop2d(void)
{

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_LIGHTING);
}

//enable orthographic view for drawing 2d controls etc.
void start2d(void)
{
	glDisable(GL_LIGHTING);
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0.0, 1.0, 1.0, 0.0);	//map the screen width/height to [0,1]
	glMatrixMode(GL_MODELVIEW);
	//glPushMatrix();
	glLoadIdentity();
}

//Made by Nils O. Selåsdal
