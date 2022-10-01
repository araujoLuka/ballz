#include "ball.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#define START_ALLOC 10

ball_t *ball_list_make(float disp_w, float disp_h, int sel)
{
    ball_t *b;
    int ball_size;
    char ball_path[150], c[2];

    ball_size = 14;
    
    if (!(b = malloc(sizeof(ball_t))))
    {
        fprintf(stderr, "Falha ao alocar estrutura de bolinhas\n");
        return NULL;
    }

    c[0] = (char)(sel + '0');
    c[1] = '\0';
    strcpy(ball_path, "./resources/models/ball_");
    strcat(ball_path, c);
    strcat(ball_path, ".png");
    b->img = al_load_bitmap(ball_path);
    b->vel = 15;
    b->radius = ball_size / 2;

    b->ini = ball_make(disp_w / 2, disp_h - b->radius);
    b->first = b->ini;

    b->tam = 1;
    b->n = 1;
    b->l_ctr = 0;
    b->l_tmr = 0;
    b->launch = false;

    return b;
}

nodo_b *ball_make(double x, double y)
{
    nodo_b *b;
    
    b = malloc(sizeof(nodo_b));
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

int ball_insert(ball_t *b, int q)
{
    nodo_b *tmp;
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
    b->n = b->tam;

    return 1;
}

void ball_launch(ball_t *b, vec_t *d, float vel)
{
    nodo_b *aux = b->ini;
    vec_t delta_norm;

    delta_norm = vector_norm(*d);

    for (b->l_ctr=0; b->l_ctr < b->tam; b->l_ctr++)
    {
        aux->sx = delta_norm.x * vel;
        aux->sy = delta_norm.y * vel;
        aux = aux->next;
    }
    b->launch = true;
}

void ball_draw(ball_t *b)
{
    nodo_b *aux;
    aux = b->ini;
    
    for (int i=0; i < b->tam; i++)
    {
        al_draw_bitmap(b->img, aux->cx - b->radius, aux->cy - b->radius, 0);
        aux = aux->next;
    }
}


int ball_collide_left(nodo_b *b, float r, int lim, ALLEGRO_SAMPLE *sound)
{
    if (b->cx - r < lim)
    {
        b->cx = lim + r;
        b->sx = -b->sx;
        al_play_sample(sound, 1, 0, 1, ALLEGRO_PLAYMODE_ONCE, NULL);
        return 1;
    }
    return 0;
}

int ball_collide_right(nodo_b *b, float r, int lim, ALLEGRO_SAMPLE *sound)
{
    if (b->cx + r > lim)
    {
        b->cx = lim - r;
        b->sx = -b->sx;
        al_play_sample(sound, 1, 0, 1, ALLEGRO_PLAYMODE_ONCE, NULL);
        return 1;
    }
    return 0;
}

int ball_collide_top(nodo_b *b, float r, int lim, ALLEGRO_SAMPLE *sound)
{
    if (b->cy - r < lim)
    {
        b->cy = lim + r;
        b->sy = -b->sy;
        al_play_sample(sound, 1, 0, 1, ALLEGRO_PLAYMODE_ONCE, NULL);
        return 1;
    }
    return 0;
}
int ball_collide_bottom(nodo_b *b, float r, int lim)
{
    if (b->cy + r > lim)
    {
        b->cy = lim - r;
        b->sx = 0;
        b->sy = 0;
        return 1;
    }
    return 0;
}

int ball_collide_block(nodo_b *b, matrix_bl *m, float radius, int *nb, int *coins, ALLEGRO_SOUNDS *s)
{
    int hits, t;
    hits = 0;

    if (b->sy < 0)
        t = block_collide_bottom(m, b, radius, nb, coins, s);
    else
        t = block_collide_top(m, b, radius, nb, coins, s);
    
    if (t == BLOCK)
        hits += 1;
    
    if (b->sx < 0)
        t = block_collide_right(m, b, radius, nb, coins, s);
    else
        t = block_collide_left(m, b, radius, nb, coins, s);
    
    if (t == BLOCK)
        hits += 1;

    return hits;
}

void ball_list_destroy(ball_t **b)
{
    nodo_b *aux;

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