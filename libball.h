#ifndef BALL_H_
#define BALL_H_
#include <allegro5/allegro5.h>
#include <allegro5/allegro_image.h>

typedef struct ball
{
    float x, y;
    float sx, sy;
    bool move;
} ball_t;

typedef struct group_balls
{
    ALLEGRO_BITMAP *img;
    ball_t **balls;
    float vel;
    float raio;
    int n, allocs;
    int l_ctr;
    bool launch;
} ball_st;

ball_st *cria_struct_ball();

ball_t *cria_ball();

ball_st *insere_ball(ball_st *b, int q);

void destroi_struct_ball(ball_st *b);

void lancar_bola(ball_t *b, float x, float y, float vel);

#endif