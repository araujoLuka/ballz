#ifndef GAME_H_
#define GAME_H_

#include "engine.h"
#include "object.h"
#include "ball.h"
#include "block.h"
#include "data.h"

typedef enum { ANIM, AIM, LAUNCH, RESET, OVER, PAUSE, RESTART, STOP } g_states;

typedef struct game
{
    matrix_bl *m;
    ball_t *b;
    box_t gm_box;
    g_states gst;
    u_data **d;
    int points, high, coins;
    float scale;
    bool paused;
} game_t;

game_t *game_make(float scr_w, float scr_h, u_data **d);

int game_start(engine_t *e, game_t **gs, u_data **d, vec_t *start, vec_t *delta);

int game_update(engine_t *e, game_t *gs, bool *mspress, vec_t *aim1, vec_t *aim2, int *nb, float *lt);

g_states game_events(engine_t *e, g_states gst, g_states lst, float *scale);

void game_anim(engine_t *e, game_t *gs);

void game_aim(engine_t *e, game_t *gs, bool *mspress, vec_t *start, vec_t *delta);

void game_launch(engine_t *e, game_t *gs, vec_t *v, float *lt, int *nb);

void game_reset(engine_t *e, game_t *gs, vec_t *v, int *nb);

void game_over(engine_t *e, game_t *gs);

void game_paused(engine_t *e, game_t *gs, g_states lst);

int game_launch_end(engine_t *e, game_t *gs, vec_t *v, int *nb);

void game_test(engine_t *e, game_t *gs, int *nb);

void game_draw(engine_t *e, game_t *gs, vec_t *vector, int anim);

void game_draw_aim(ball_t *b, float dx, float dy);

void game_draw_pause(engine_t *e, vec_t mouse_pos, box_t gme_b, box_t rsm_b, box_t rst_b, box_t menu_b);

void game_draw_over(engine_t *e, game_t *gs, vec_t mouse_pos, box_t rst_b, box_t mnu_b, bool new_best);

void game_destroy(game_t **gs);

#endif