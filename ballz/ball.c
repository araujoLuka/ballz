#include "ball.h"
#include "block.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#define START_ALLOC 10
#include <allegro5/allegro5.h>
#include <allegro5/allegro_image.h>

ALLEGRO_BITMAP *load_bitmap_at_size(const char *filename, int w, int h);

list_b *ball_list_make(float disp_w, float disp_h)
{
    list_b *b;
    
    if (!(b = malloc(sizeof(list_b))))
    {
        fprintf(stderr, "Falha ao alocar estrutura de bolinhas\n");
        return NULL;
    }

    b->img = load_bitmap_at_size("./resources/bola.png", 15, 15);
    b->vel = 1000;
    b->raio = al_get_bitmap_width(b->img) / 2;

    b->ini = ball_make(disp_w / 2, disp_h - b->raio);

    b->tam = 1;
    b->l_ctr = 0;
    b->launch = false;

    return b;
}

ball_t *ball_make(double x, double y)
{
    ball_t *b;
    
    b = malloc(sizeof(ball_t));
    if (!b)
        return NULL;
        
    b->cx = x;
    b->cy = y;
    b->sx = 0;
    b->sy = 0;
    b->move = false;
    b->next = NULL;

    return b;
}

int ball_insert(list_b *b, int q)
{
    ball_t *tmp;
    tmp = b->ini;

    for (int i=0; i < q; i++)
    {
        if (!(tmp = ball_make(b->ini->cx, b->ini->cy)))
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

void ball_launch(list_b *b, float x, float y, float vel)
{
    ball_t *aux = b->ini;
    float hip, vel_x, vel_y;
    hip = sqrt(x*x + y*y);

    if (y > -20 || hip == 0)
        return;

    vel_x = (x / hip) * vel * 1000;
    vel_y = (y / hip) * vel * 1000;

    for (b->l_ctr=0; b->l_ctr < b->tam; b->l_ctr++)
    {
        aux->sx = vel_x;
        aux->sy = vel_y;
        aux = aux->next;
    }
    b->launch = true;
}

int ball_collide_left(ball_t *b, float r, int lim)
{
    if (b->cx - r < lim)
    {
        b->cx = lim + r;
        b->sx = -b->sx;
        return 1;
    }
    return 0;
}

int ball_collide_right(ball_t *b, float r, int lim)
{
    if (b->cx - r > lim)
    {
        b->cx = lim + r;
        b->sx = -b->sx;
        return 1;
    }
    return 0;
}

int ball_collide_top(ball_t *b, float r, int lim)
{
    if (b->cy - r < lim)
    {
        b->cy = lim + r;
        b->sy = -b->sy;
        return 1;
    }
    return 0;
}
int ball_collide_bottom(ball_t *b, float r, int lim)
{
    if (b->cy - r > lim)
    {
        b->cy = lim + r;
        b->sx = -b->sx;
        b->sy = -b->sy;
        return 1;
    }
    return 0;
}

int ball_collide_block(ball_t *b, void *ptr_m, float r, int *nb, int *c)
{
    matrix_bl *m = ptr_m;
    block_t *bl;
    int i, j, ret = 0;

    if (ptr_m == NULL)
        return ret;

    i = (int)(b->cy / (m->size))-1;
    j = (int)(b->cx / (m->size));

    if (b->sy < 0)
    {
        if (i-1 >= 0 && i-1 < ROWS_B && j >= 0 && j < COLS_B && (bl = m->bl[i-1][j]) != NULL)
            if (ball_collide_top(b, r, bl->h))
            {
                ret++;
                bl->valor--;
                if (bl->valor == 0)
                    m->bl[i-1][j] = NULL;
                switch (bl->tipo)
                {
                    case 2:
                        (*c)++;
                        b->sy = -b->sy;
                        break;
                    case 3:
                        (*nb)++;
                        b->sy = -b->sy;
                        break;
                }
            }
    }
    else
    {
        if (i+1 >= 0 && i+1 < ROWS_B && j > 0 && j < COLS_B && (bl = m->bl[i+1][j]) != NULL)
            if (ball_collide_bottom(b, r, bl->y))
            {
                ret++;
                bl->valor--;
                if (bl->valor == 0)
                    m->bl[i+1][j] = NULL;
                switch (bl->tipo)
                {
                    case 2:
                        (*c)++;
                        b->sy = -b->sy;
                        break;
                    case 3:
                        (*nb)++;
                        b->sy = -b->sy;
                        break;
                }
            }
    }

    if (b->sx < 0)
    {
        if (j-1 >= 0 && j-1 < COLS_B && i >= 0 && i < ROWS_B && (bl = m->bl[i][j-1]) != NULL)
            if (ball_collide_left(b, r, bl->w))
            {
                ret++;
                bl->valor--;
                if (bl->valor == 0)
                    m->bl[i][j-1] = NULL;
                switch (bl->tipo)
                {
                    case 2:
                        (*c)++;
                        b->sx = -b->sx;
                        break;
                    case 3:
                        (*nb)++;
                        b->sx = -b->sx;
                        break;
                }
            }
    }
    else
    {
        if (j >= 0 && j < COLS_B && i >= 0 && i < ROWS_B && (bl = m->bl[i][j]) != NULL)
        {
            if (ball_collide_right(b, r, bl->x - m->space))
            {
                ret++;
                bl->valor--;
                if (bl->valor == 0)
                    m->bl[i][j] = NULL;
                switch (bl->tipo)
                {
                    case 2:
                        (*c)++;
                        b->sx = -b->sx;
                        break;
                    case 3:
                        (*nb)++;
                        b->sx = -b->sx;
                        break;
                }
            }
        }
    }

    return ret;
}

void ball_list_destroy(list_b **b)
{
    ball_t *aux;

    for (; (*b)->tam > 0; (*b)->tam--)
    {
        aux = (*b)->ini;
        (*b)->ini = aux->next;
        free(aux);
    }

    al_destroy_bitmap((*b)->img);
    free(*b);
    *b = NULL;
}