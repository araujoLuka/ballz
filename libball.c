#include "libball.h"
#include <stdlib.h>
#include <math.h>
#define START_ALLOC 10

ALLEGRO_BITMAP *load_bitmap_at_size(const char *filename, int w, int h);

ball_st *cria_struct_ball()
{
    ball_st *b;

    b = malloc(sizeof(ball_st));
    if (!b)
        return NULL;
    
    b->balls = malloc(sizeof(ball_t*)*START_ALLOC);
    if (!b->balls)
    {
        free(b);
        return NULL;
    }

    b->balls[0] = cria_ball();

    b->img = load_bitmap_at_size("./resources/bola.png", 22, 22);
    b->vel = 15;
    b->raio = al_get_bitmap_width(b->img) / 2;

    b->allocs = START_ALLOC;
    b->n = 1;
    b->l_ctr = 0;
    b->launch = false;

    return b;
}

ball_t *cria_ball()
{
    ball_t *b;
    
    b = malloc(sizeof(ball_t));
    if (!b)
        return NULL;
        
    b->x = 0;
    b->y = 0;
    b->sx = 0;
    b->sy = 0;
    b->move = false;

    return b;
}

ball_st *insere_ball(ball_st *b, int q)
{
    if (b->n + q >= b->allocs)
    {
        b->balls = realloc(b->balls, sizeof(ball_t*) * (b->allocs * 2 + q));
        if (!b->balls)
            return NULL;
        b->allocs = b->allocs * 2 + q;
    }

    for (int i = b->n; i < b->n + q; i++){
        b->balls[i] = cria_ball();
        b->balls[i]->x = b->balls[0]->x;
        b->balls[i]->y = b->balls[0]->y;
    }

    b->n = b->n + q;

    return b;
}

void destroi_struct_ball(ball_st *b)
{
    al_destroy_bitmap(b->img);
    
    for(int i=0; i < b->n; i++)
        free(b->balls[i]);
    
    free(b->balls);
    free(b);
    b = NULL;
}

void lancar_bola(ball_t *b, float x, float y, float vel)
{
    float hip;

    if (y > -20)
    {
        b->sx = 0;
        b->sy = 0;
    }
    else
    {
        hip = sqrt(x*x + y*y);
        if (hip == 0)
            return;
        b->sx = (x / hip) * vel;
        b->sy = (y / hip) * vel;
    }
}