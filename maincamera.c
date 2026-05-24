#include <SDL.h>
#include "common.h"
#include "maincamera.h"
#include "input.h"
#define MOVE_STEP 16.00

Camera mainCamera;

void move_main_camera(void)
{
    double delta = timer_last_delta(&frameTimer);
    if (key_state(SDLK_w)) {
        camera_truck_z(&mainCamera, -MOVE_STEP * delta);
    }
    if (key_state(SDLK_s)) {
        camera_truck_z(&mainCamera, MOVE_STEP * delta);
    }
    if (key_state(SDLK_a)) {
        camera_truck_x(&mainCamera, -MOVE_STEP * delta);
    }
    if (key_state(SDLK_d)) {
        camera_truck_x(&mainCamera, MOVE_STEP * delta);
    }
    if (key_state(SDLK_q)) {
        camera_roll(&mainCamera, MOVE_STEP * delta);
    }
    if (key_state(SDLK_e)) {
        camera_roll(&mainCamera, -MOVE_STEP * delta);
    }
    if (key_state(SDLK_h)) {
        camera_turn(&mainCamera, 2 * -MOVE_STEP * delta);
    }
    if (key_state(SDLK_l)) {
        camera_turn(&mainCamera, 2 * MOVE_STEP * delta);
    }
    if (key_state(SDLK_j)) {
        camera_truck_y(&mainCamera, -MOVE_STEP * delta);
    }
    if (key_state(SDLK_k)) {
        camera_truck_y(&mainCamera, MOVE_STEP * delta);
    }
}

// Made by Nils O. Selåsdal
