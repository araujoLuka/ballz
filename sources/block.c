#include "block.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>

matrix_bl *block_matrix_make(float g_w)
{
    matrix_bl *m;

    if (!(m = malloc(sizeof(matrix_bl))))
    {
        fprintf(stderr, "Falha ao criar matriz de blocos\n");
        return NULL;
    }

    for (int i=0; i < ROWS_B; i++)
        for (int j=0; j < COLS_B; j++)
            m->bl[i][j] = NULL;

    block_rwc_make(m->rows);
    m->space = 4;
    m->size = g_w / (COLS_B);
    m->coin_img = al_load_bitmap("resources/models/coin1.png");

    return m;
}

void block_rwc_make(row_control *r)
{
    for (int i=0; i < ROWS_B; i++)
    {
        r[i].n_blocks = 0;
        r[i].have_coin = false;
        r[i].have_ball = false;
    }
}

int aleat (int ini, int fim)
{
    int tam, x_aleat;
    
    tam = fim - ini;
    x_aleat = ini + (rand() % tam);
    
    return x_aleat;
}

nodo_bk *block_make(float x, float y, float size, int space, int points, row_control *r)
{
    int prob, bonus_b;
    prob = aleat(0, 100);
    bonus_b = 0;

    if (r[0].n_blocks < 2)
        bonus_b += 5;
    if (r[1].n_blocks <= 2)
        bonus_b += 5;
    
    if (points > 1 && !r[0].have_coin && prob <= PROB_COIN)
        return block_type_coin(x, y, size, space + size*0.2, &r[0].have_coin);
    
    if (r[0].n_blocks < COLS_B-2 && prob <= PROB_BLOCK + bonus_b)
        return block_type_block(x, y, size, space, points, &r[0].n_blocks);

    return NULL;
}

nodo_bk *block_type_block(float x, float y, float size, int space, int points, int *n_blocks)
{
    nodo_bk *bl;
    
    bl = malloc(sizeof(nodo_bk));
    bl->x = x + space;
    bl->y = y + space;
    bl->x2 = x + size - space;
    bl->y2 = y + size - space;
    bl->cx = x + size/2;
    bl->cy = y + size/2;
    bl->t = BLOCK;
    if (aleat(0, 100) <= 10)
        points *= 2;
    bl->value = points;
    bl->hit = 0;
    *n_blocks += 1;

    return bl;
}

nodo_bk *block_type_coin(float x, float y, float size, int space, bool *h_coin)
{
    nodo_bk *bl;
    
    bl = malloc(sizeof(nodo_bk));
    bl->x = x + space;
    bl->y = y + space;
    bl->x2 = x + size - space;
    bl->y2 = y + size - space;
    bl->cx = x + size/2;
    bl->cy = y + size/2;
    bl->t = COIN;
    bl->value = 1;
    bl->hit = 0;
    *h_coin = true;

    return bl;
}

nodo_bk *block_type_ball(float x, float y, float size, int space, bool *h_ball)
{
    nodo_bk *bl;
    
    bl = malloc(sizeof(nodo_bk));
    bl->x = x + space;
    bl->y = y + space;
    bl->x2 = x + size - space;
    bl->y2 = y + size - space;
    bl->cx = x + size/2;
    bl->cy = y + size/2;
    bl->t = BALL;
    bl->value = 1;
    bl->hit = 0;
    *h_ball = true;

    return bl;
}

void block_insert_row(matrix_bl *m, float x, float y, int points)
{
    int j;

    for (j=0; j < COLS_B; j++)
        m->bl[0][j] = block_make(x + m->size*j, y, m->size, m->space, points, m->rows);

    if (points > 1 && aleat(1, 100) <= PROB_BALL)
    {
        j = aleat(0, COLS_B);
        while (m->bl[0][j] != NULL && m->bl[0][j]->t == COIN)
            j = aleat(0, COLS_B);
        
        if (m->bl[0][j] != NULL && m->bl[0][j]->t == BLOCK)
            m->rows[0].n_blocks--;
        
        block_destroy(&m->bl[0][j]);
        m->bl[0][j] = block_type_ball(x + m->size*j, y, m->size, m->space + m->size*0.2, &m->rows[0].have_ball);
    }
}

int block_move(matrix_bl *m)
{
    int i = 0;
    int j = 0;

    for (i=ROWS_B-1; i > 0; i--)
    {
        for (j=0; j < COLS_B; j++)
        {
            if (m->bl[i-1][j] != NULL)
            {
                m->bl[i-1][j]->y += m->size;
                m->bl[i-1][j]->y2 += m->size;
                m->bl[i-1][j]->cy += m->size;
            }
            m->bl[i][j] = m->bl[i-1][j];
        }
        m->rows[i].have_coin = m->rows[i-1].have_coin;
        m->rows[i].have_ball = m->rows[i-1].have_ball;
        m->rows[i].n_blocks = m->rows[i-1].n_blocks;
    }

    for (i=0; i < COLS_B; i++)
        m->bl[0][i] = NULL;
    m->rows[0].n_blocks = 0;
    m->rows[0].have_coin = false;
    m->rows[0].have_ball = false;

    return 1;
}

// Tratamento para colisao com o bloco, retorna o tipo do bloco
type_b block_resolve_collide(nodo_bk **bl, int *new_balls, int *coins, row_control *r, ALLEGRO_SOUNDS *s)
{
    type_b type = (*bl)->t; // copia do tipo em caso de free(*bl)

    switch (type)
    {
    case BLOCK:
        // Registra o acerto
        (*bl)->hit++;

        // Som de acerto do bloco
        al_play_sample(s->hit_block, 1, 0, 1, ALLEGRO_PLAYMODE_ONCE, NULL);

        // Destroi o bloco quando o total de hits atingir o valor
        if ((*bl)->hit >= (*bl)->value) 
        {
            block_destroy(bl);
            r->n_blocks--;
        }
        break;

    case COIN:
        // Coleta a moeda
        *coins += 1;

        // Som da coleta da moeda
        al_play_sample(s->coin, 1, 0, 1, ALLEGRO_PLAYMODE_ONCE, NULL);

        block_destroy(bl);
        r->have_coin = false;
        break;

    case BALL:
        // Coleta a bolinha extra
        *new_balls += 1;

        // Som da coleta da bolinha extra
        al_play_sample(s->ball_up, 1, 0, 1, ALLEGRO_PLAYMODE_ONCE, NULL);

        block_destroy(bl);
        r->have_ball = false;
        break;

    default:
        break;
    }

    return type;
}

bool block_collision(nodo_bk *bl, nodo_b *b, float radius)
{
    vec_t nearest_point, ball_block_dist, norm_dist, ball_vel, aux;
    float overlap;

    ball_vel = vector_make(b->sx, b->sy);
    nearest_point = vector_make(fmaxf(bl->x, fminf(b->cx, bl->x2)), fmaxf(bl->y, fminf(b->cy, bl->y2)));
    ball_block_dist = vector_dif(nearest_point, vector_make(b->cx, b->cy));
    norm_dist = vector_norm(ball_block_dist);

    overlap = fmaxf(radius, vector_len(ball_vel)) - vector_len(ball_block_dist);

    if (overlap > 0)
    {
        if (bl->t == BLOCK)
        {
            b->cx += -norm_dist.x * overlap;
            b->cy += -norm_dist.y * overlap;
            
            if (b->cx - radius/2 > bl->x && b->cx + radius/2 < bl->x2)
                b->sy = -b->sy;
            else if (b->cy - radius/2 > bl->y && b->cy + radius/2 < bl->y2)
                b->sx = -b->sx;
            else
            {
                if (b->cx + radius/2 >= bl->x && b->cx + radius/2 < bl->x2)
                    if (b->cy - radius/2 <= bl->y2 && b->cy - radius/2 > bl->y)
                    {
                        aux = vector_norm(vector_make(b->cx-radius-bl->x, b->cy+radius-bl->y2));
                        b->sx = aux.x * 15;
                        b->sy = aux.y * 15;
                    }
                    else
                    {
                        aux = vector_norm(vector_make(b->cx-radius-bl->x, b->cy-radius-bl->y2));
                        b->sx = aux.x * 15;
                        b->sy = aux.y * 15;
                    }
                else
                    if (b->cy - radius/2 <= bl->y2 && b->cy - radius/2 > bl->y)
                    {
                        aux = vector_norm(vector_make(b->cx+radius-bl->x, b->cy+radius-bl->y2));
                        b->sx = aux.x * 15;
                        b->sy = aux.y * 15;
                    }
                    else
                    {
                        aux = vector_norm(vector_make(b->cx+radius-bl->x, b->cy-radius-bl->y2));
                        b->sx = aux.x * 15;
                        b->sy = aux.y * 15;
                    }
                if (b->sy > 0 && b->sy < 2)
                    b->sy = 3;
                else if (b->sy < 0 && b->sy > -2)
                    b->sy = -3;
            } 
        }
        return true;
    }

    return false;
}

type_b block_collide_bottom(matrix_bl *m, nodo_b *b, float radius, int *nb, int *coins, ALLEGRO_SOUNDS *s)
{
    nodo_bk *bl;
    int c, r;

    if ((bl = block_get_by_quadrant(m, b->cx, b->cy - m->size/2, &r, &c)))
    {
        if (block_collision(bl, b, radius))
            return block_resolve_collide(&m->bl[r][c], nb, coins, &m->rows[r], s);
    }

    return NONE;
}

type_b block_collide_left(matrix_bl *m, nodo_b *b, float radius, int *nb, int *coins, ALLEGRO_SOUNDS *s)
{
    nodo_bk *bl;
    int c, r;

    if ((bl = block_get_by_quadrant(m, b->cx + m->size/2, b->cy, &r, &c)))
    {
        if (block_collision(bl, b, radius))
            return block_resolve_collide(&m->bl[r][c], nb, coins, &m->rows[r], s);
    }

    return NONE;
}

type_b block_collide_top(matrix_bl *m, nodo_b *b, float radius, int *nb, int *coins, ALLEGRO_SOUNDS *s)
{
    nodo_bk *bl;
    int c, r;
    
    if ((bl = block_get_by_quadrant(m, b->cx, b->cy + m->size/2, &r, &c)))
    {
        if (block_collision(bl, b, radius))
            return block_resolve_collide(&m->bl[r][c], nb, coins, &m->rows[r], s);
    } 

    return NONE;
}

type_b block_collide_right(matrix_bl *m, nodo_b *b, float radius, int *nb, int *coins, ALLEGRO_SOUNDS *s)
{
    nodo_bk *bl;
    int c, r;

    if ((bl = block_get_by_quadrant(m, b->cx - m->size/2, b->cy, &r, &c)))
    {
        if (block_collision(bl, b, radius))
            return block_resolve_collide(&m->bl[r][c], nb, coins, &m->rows[r], s);
    } 

    return NONE;
}

nodo_bk *block_get_by_quadrant(matrix_bl *m, float x, float y, int *i, int *j)
{
    *i = (int)((y-87) / (m->size));
    *j = (int)((x-7) / (m->size));

    if (*i < 0 || *i >= ROWS_B || *j < 0 || *j >= COLS_B)
        return NULL;
    
    return m->bl[*i][*j];
}

ALLEGRO_COLOR block_color(int value, int hits)
{
    int r, g, b;
    int x, div;
    x = value - hits;

    if (x <= 5)
    {
        r = 245;
        g = 181;
        b =  47;
    }
    else if (x <= 15)
    {
        div = 15 - 6;
        r = 130 + ((245-130)/div)*hits;
        g = 197 + ((181-197)/div)*hits;
        b =  65 + (( 47- 65)/div)*hits;
    }
    else if (x <= 29)
    {
        div = 29 - 16;
        r = 234 + ((130-234)/div)*hits;
        g =  35 + ((197- 35)/div)*hits;
        b =  94 + (( 65- 94)/div)*hits;
    }
    else if (x <= 49)
    {
        div = 49 - 30;
        r =  22 + ((234- 22)/div)*hits;
        g = 116 + (( 35-116)/div)*hits;
        b = 188 + (( 94-188)/div)*hits;
    }
    else
    {
        r =   0;
        g = 154;
        b = 140;
    }

    return al_map_rgb(r, g, b);
}

void block_draw(engine_t *e, matrix_bl *m, int anim)
{
    ALLEGRO_COLOR BG_COLOR;
    ALLEGRO_COLOR BL_COLOR;
    nodo_bk *b;
    float aux;

    BG_COLOR = al_map_rgb(20, 20, 20);
    for (int i=0; i < ROWS_B; i++)
        for(int j=0; j < COLS_B; j++)
        {
            if ((b = m->bl[i][j]))
            {
                switch (b->t)
                {
                case BLOCK:
                    BL_COLOR = block_color(b->value, b->hit);
                    al_draw_filled_rounded_rectangle(b->x, b->y - anim, b->x2, b->y2 - anim, 2, 2, BL_COLOR);
                    
                    draw_num(e->fonts->r22, BG_COLOR, b->cx, b->cy-anim, -1, b->value - b->hit);
                    break;

                case COIN:
                    BL_COLOR = al_map_rgb(240,240,70);
                    al_draw_bitmap(m->coin_img, b->x, b->y - anim, 0);
                    break;

                case BALL:
                    BL_COLOR = al_map_rgb(255,255,255);
                    aux = (b->x2 - b->x)/2;
                    al_draw_filled_circle(b->x + aux, b->y + aux - anim, aux, BL_COLOR);
                    al_draw_filled_circle(b->x + aux, b->y + aux - anim, aux * 0.85, BG_COLOR);
                    al_draw_filled_circle(b->x + aux, b->y + aux - anim, aux * 0.4, BL_COLOR);
                    break;

                default:
                    break;  
                }
            }
        }
}

void block_destroy(nodo_bk **bl)
{
    free(*bl);
    *bl = NULL;
}

void block_matrix_destroy(matrix_bl **m)
{
    for (int i=0; i < ROWS_B; i++)
    {
        for(int j=0; j < COLS_B; j++)
        {
            block_destroy(&(*m)->bl[i][j]);
        }
    }

    free(*m);
    *m = NULL;
}