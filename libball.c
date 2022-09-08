#include "libball.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#define START_ALLOC 10
#include <allegro5/allegro5.h>
#include <allegro5/allegro_image.h>

ALLEGRO_BITMAP *load_bitmap_at_size(const char *filename, int w, int h);

list_b *make_ball_list(float disp_w, float disp_h)
{
    list_b *b;
    
    if (!(b = malloc(sizeof(list_b))))
    {
        fprintf(stderr, "Falha ao alocar estrutura de bolinhas\n");
        return NULL;
    }

    b->img = load_bitmap_at_size("./resources/bola.png", 18, 18);
    b->vel = 1000;
    b->raio = al_get_bitmap_width(b->img) / 2;

    b->ini = cria_ball(disp_w / 2, disp_h - b->raio * 2);

    b->tam = 1;
    b->l_ctr = 0;
    b->launch = false;

    return b;
}

ball_t *cria_ball(double x, double y)
{
    ball_t *b;
    
    b = malloc(sizeof(ball_t));
    if (!b)
        return NULL;
        
    b->x = x;
    b->y = y;
    b->sx = 0;
    b->sy = 0;
    b->move = false;
    b->next = NULL;

    return b;
}

int insert_ball(list_b *b, int q)
{
    ball_t *tmp;
    tmp = b->ini;

    for (int i=0; i < q; i++)
    {
        if (!(tmp = cria_ball(b->ini->x, b->ini->y)))
        {
            fprintf(stderr, "Falha ao inserir nova bolinha\n");
            return 0;
        }
        tmp->next = b->ini;
        b->ini = tmp;
    }

    b->tam = b->tam + q;

    return 1;
}

void collide_left(ball_t *b, int lim)
{
    if (b->x < lim)
    {
        b->x = lim;
        b->sx = -b->sx;
    }
}

void collide_right(ball_t *b, int lim)
{
    if (b->x > lim)
    {
        b->x = lim;
        b->sx = -b->sx;
    }
}

void collide_top(ball_t *b, int lim)
{
    if (b->y < lim)
    {
        b->y = lim;
        b->sy = -b->sy;
    }

}
void collide_bottom(ball_t *b, int lim)
{
    if (b->y > lim)
    {
        b->y = lim;
        b->sx = 0;
        b->sy = 0;
    }
}

void set_ball_speed(ball_t *b, float x, float y, float vel)
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

list_b *destroy_ball_list(list_b *b)
{
    ball_t *aux;

    for (; b->tam > 0; b->tam--)
    {
        aux = b->ini;
        b->ini = aux->next;
        free(aux);
    }

    al_destroy_bitmap(b->img);
    free(b);

    return NULL;
}