#ifndef GAME_H_
#define GAME_H_

#include "engine.h"
#include "object.h"
#include "ball.h"
#include "block.h"

typedef enum { ANIM, AIM, LAUNCH, RESET, OVER, PAUSE, RESTART } g_states;

typedef struct game
{
    box_t *g;
    matrix_bl *m;
    ball_t *b;
    int points, coins;
    float curr_t, last_t, delta_t, t_mult;
    bool paused;
} game_t;

game_t *game_make(float scr_w, float scr_h);

int game_start(engine_t *e, game_t **gs, vet_t *start, vet_t *delta, g_states *gst);

int game_update(engine_t *e, game_t *gs, bool *mspress, vet_t *aim1, vet_t *aim2, int *nb, float *lt, g_states *gst);

g_states game_events(engine_t *e, g_states gst, g_states lst, float *scale);

g_states game_aim(engine_t *e, game_t *gs, bool *mspress, vet_t *start, vet_t *delta);

g_states game_launch(engine_t *e, game_t *gs, vet_t *v, float *lt, int *nb);

int game_launch_end(engine_t *e, game_t *gs, vet_t *v, int *nb);

g_states game_reset(game_t *gs);

void game_test(engine_t *e, game_t *gs, int *nb);

void game_draw(engine_t *e, game_t *gs, vet_t *vector, g_states gst);

void game_aim_draw(ball_t *b, float dx, float dy);

void game_destroy(game_t **gs);

#endif