#include <SDL.h>
#include <SDL_image.h>
#include "larena.h"
#if defined(__APPLE__)
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include "common.h"
#include "init.h"
#include "input.h"
#include "timer.h"
#include "sokoban.h"
#include "vector.h"

Timer frameTimer;

void do_quit(int code)
{
    exit(code);
}

static void install_signals()
{
    signal(SIGTERM, do_quit);
    signal(SIGINT, do_quit);  // ctrl+c
#ifdef __unix
    signal(SIGHUP, do_quit);
    signal(SIGQUIT, do_quit);
    signal(SIGSEGV, do_quit);  // segmentation failt

#endif
}

typedef struct move_animation move_animation;
struct move_animation {
    vec3_t from;
    vec3_t to;
    double elapsed;
    double duration;
    int running;
};
int main(int argc, char *argv[])
{
    /* Information about the current video settings. */
    double fpslast = 0.0f;
    int frames = 0;
    const char *levelfile;
    LArena game_arena;

    larena_init(&game_arena, malloc(GAME_ARENA_SZ), GAME_ARENA_SZ);
    install_signals();
    init();
    init_fonts(&game_arena);
    timer_init(&frameTimer);
    atexit(SDL_Quit);
    GameData game;
    init_game(&game, &game_arena);
    if (argc > 1) {
        levelfile = argv[1];
    } else
        levelfile = MAINDIR "/levels/easy.slc";

    if (load_levels(&game, levelfile) != 0) {
        fprintf(stderr, "Could not load level %s\n", levelfile);
        exit(3);
    }
    set_level(&game, 0);
    load_textures(&game);
    srand((unsigned int)SDL_GetTicks());
    frame_init(&game);
    frame_newlevel(&game);
    printf("Arena used %zu bytes\n", larena_used(&game_arena));
    /* main loop */
    move_animation anim;
    for (;;) {
        enum K_Command cmd;
        int moved = 0, finished = 0;
        timer_update(&frameTimer);
        /* Process incoming events. */
        vec3_t player_pos = {.x = game.currentLevel.playerPos.x, .y = game.currentLevel.playerPos.y, .z = 0};

        cmd = process_events();
        switch (cmd) {
            case WINDOW_RESIZED: {
                int w, h;
                SDL_GetWindowSize(mainWindow, &w, &h);
                set_view(w, h);
            } break;
            case MOVE:
                moved = move(&game.currentLevel, get_move());
                break;
            case RESTART_LVL:
                restart_level(&game);
                frame_newlevel(&game);
                break;
            case NEXT_LVL:
                next_level(&game);
                frame_newlevel(&game);
                break;
            case NOTHING:
            default:
                break;
        }

        if (moved) {
            game.nr_moves++;
            finished = check_goal(&game.currentLevel);
            anim.from = player_pos;
            vec3_t to = {.x = game.currentLevel.playerPos.x, .y = game.currentLevel.playerPos.y, .z = 0};
            anim.to = to;
            anim.elapsed = 0;
            anim.duration = 0.5;
            anim.running = 1;
        }

        if (anim.running) {
            vec3_t pos;
            double dt = timer_last_delta(&frameTimer);
            anim.elapsed += dt;
            vec3_lerp(&anim.from, &anim.to, &pos, dt / anim.duration);
            printf("Player anim pos [%f, %f, %f]\n", pos.x, pos.y, pos.z);
            if (anim.elapsed >= anim.duration) {
                anim.running = 0;
            }
        }
        /* Draw the screen. */
        if (finished) {
            Timer finisht;
            timer_init(&finisht);
            do {
                frame_begin(&game, finished);
                frames++;
                fpslast += timer_last_delta(&frameTimer);
                timer_update(&frameTimer);

            } while (timer_since_started(&finisht) <= 3.1415f);  // for 3.1415 seconds, stall and draw finished
            next_level(&game);
            frame_newlevel(&game);
        } else {
            frame_begin(&game, finished);
            frames++;
            fpslast += timer_last_delta(&frameTimer);
        }

        if (fpslast >= 4.0f) {
            DEBUG("fps %f\n", frames / 4.0f);
            fpslast = 0.0f;
            frames = 0;
        }
    }

    /* Never reached. */
    return 0;
}

// Made by Nils O. Selåsdal
