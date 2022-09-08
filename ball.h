#ifndef BALL_H_
#define BALL_H_

typedef struct balls ball_t;
typedef struct nodo_ball nodo_b;

#include "engine.h"
#include "block.h"
#include "object.h"

struct nodo_ball
{
    float cx, cy, sx, sy;
    int move;
    nodo_b *next;
};

struct balls
{
    ALLEGRO_BITMAP *img;
    float raio;
    nodo_b *ini;
    int tam, n;
    int vel;
    int l_ctr;
    float l_tmr;
    bool launch;
};

ball_t *ball_list_make(float disp_w, float disp_h);

nodo_b *ball_make(double x, double y);

int ball_insert(ball_t *b, int q);

void ball_launch(ball_t *b, vet_t *d, float scale);

void ball_draw(ball_t *b);

int ball_collide_left(nodo_b *b, float r, int lim);

int ball_collide_right(nodo_b *b, float r, int lim);

int ball_collide_top(nodo_b *b, float r, int lim);

int ball_collide_bottom(nodo_b *b, float r, int lim);

int ball_collide_block(nodo_b *b, matrix_bl *m, float r, int *nb, int *c);

void ball_list_destroy(ball_t **b);

#endif