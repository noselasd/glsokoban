#include <string.h>
#include "camera.h"

void camera_init(Camera * cam)
{
	cam->orientation.angle = 1.0f;
	cam->orientation.x = 0.0f;
	cam->orientation.y = 0.0f;
	cam->orientation.z = 0.0f;
	cam->position.x = 0.0f;
	cam->position.y = 0.0f;
	cam->position.z = 0.0f;
}
void camera_truck_x(Camera * cam, float truckby)
{
	quat_t tmp, tmp2;
	vec3_t dir;
	quat_copy(&cam->orientation, &tmp);
	quat_set3(&tmp2, 0.0f, -90.0f, 0.0f);
	quat_postmult(&tmp, &tmp2);
	quat_direction(&tmp, &dir);
	vec3_scale(&dir, truckby, &dir);
	vec3_add(&cam->position, &dir, &cam->position);

}
void camera_turn(Camera * cam, float rollby)
{
	quat_t tmp;
	quat_set3(&tmp, 0.0f, rollby, 0.0f);
	quat_postmult(&cam->orientation, &tmp);
}
void camera_roll(Camera * cam, float rollby)
{
	quat_t tmp;
	quat_set3(&tmp, 0.0f, 0.0f, rollby);
	quat_postmult(&cam->orientation, &tmp);
}
void camera_truck_y(Camera * cam, float truckby)
{
	quat_t tmp, tmp2;
	vec3_t dir;
	quat_copy(&cam->orientation, &tmp);
	quat_set3(&tmp2, 90.0f, 0.0f, 0.0f);
	quat_postmult(&tmp, &tmp2);
	quat_direction(&tmp, &dir);
	vec3_scale(&dir, truckby, &dir);
	vec3_add(&cam->position, &dir, &cam->position);

}
void camera_truck_z(Camera * cam, float truckby)
{
	vec3_t dir;
	quat_direction(&cam->orientation, &dir);
	vec3_scale(&dir, truckby, &dir);
	vec3_add(&cam->position, &dir, &cam->position);

}

//Made by Nils O. Selåsdal
