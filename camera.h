#ifndef __CAMERA_H__
#define __CAMERA_H__
#include "vector.h"
#include "quat.h"
typedef struct Camera Camera;


struct Camera {
	vec3_t position;
	quat_t orientation;
};

void camera_init(Camera * cam);
void camera_truck_x(Camera * cam, float truckby);
void camera_truck_y(Camera * cam, float truckby);
void camera_roll(Camera * cam, float rollby);
void camera_turn(Camera * cam, float rollby);
void camera_truck_z(Camera * cam, float truckby);

#endif
