#ifndef GAME_H_
#define GAME_H_

#include "engine.h"
#include "object.h"
#include "ball.h"
#include "block.h"

typedef struct game
{
    box_t *g;
    matrix_bl *m;
    ball_t *b;
    int points, t_mult, coins;
    float curr_t, last_t, delta_t;
} game_t;

game_t *game_make(float scr_w, float scr_h);

void game_destroy(game_t **gs);

int game_events(engine_t *e);

int game_aim(game_t *gs, engine_t *e, ball_t *b, bool *mspress, vet_t *start, vet_t *delta);

int game_start(engine_t *e, game_t **gs);

void game_test(engine_t *e, game_t *gs, int *nb);

int game_update(engine_t *e, game_t *gs, bool *mspress, vet_t *aim1, vet_t *aim2, int *nb, float *lt);

void game_state(engine_t *e);

#endif