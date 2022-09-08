#include "block.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#define PROB_BLOCK 65
#define PROB_BALL 80
#define PROB_GOLD 10

/* 
 * Calcula um valor aleatorio entre ini e fim e o retorna 
*/
int aleat (int ini, int fim)
{
    int tam, x_aleat;
    
    tam = fim - ini;
    x_aleat = ini + (rand() % tam);
    
    return x_aleat;
}

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

    if (!(m->rw_c = block_rwc_make()))
    {
        fprintf(stderr, "Falha ao criar linha de controle de blocos\n");
        free(m);
        return NULL;
    }

    m->space = 1;
    m->size = g_w / (COLS_B);

    return m;
}

row_control *block_rwc_make()
{
    row_control *r;

    if (!(r = malloc(sizeof(row_control))))
        return NULL;

    for (int i=0; i < ROWS_B; i++)
    {
        r->n_blocks[i] = 0;
        r->n_coins[i] = 0;
        r->n_balls[i] = 0;
    }

    return r;
}

nodo_bk *block_make(float x, float y, float size, int space, int points, row_control *r)
{
    int prob, bonus_b;
    prob = aleat(0, 100);
    bonus_b = 0;

    if (r->n_blocks[0] < 2)
        bonus_b += 5;
    if (r->n_blocks[1] < 2)
        bonus_b += 5;
    
    if (points > 1 && r->n_coins[0] == 0 && prob <= PROB_GOLD)
        return block_type_2(x, y, size, space + size*0.3, &r->n_coins[0]);
    
    if (r->n_blocks[0] < COLS_B-1 && prob <= PROB_BLOCK + bonus_b)
        return block_type_1(x, y, size, space, points, &r->n_blocks[0]);

    return NULL;
}

nodo_bk *block_type_1(float x, float y, float size, int space, int points, int *rw_ctr)
{
    nodo_bk *bl;
    
    bl = malloc(sizeof(nodo_bk));
    bl->x = x + space;
    bl->y = y + space;
    bl->w = x + size - space;
    bl->h = y + size - space;
    bl->cx = x + size/2;
    bl->cy = y + size/2;
    bl->tipo = 1;
    if (aleat(0, 100) <= 10)
        points *= 2;
    bl->valor = points;
    *rw_ctr += 1;

    return bl;
}

nodo_bk *block_type_2(float x, float y, float size, int space, int *rw_ctr)
{
    nodo_bk *bl;
    
    bl = malloc(sizeof(nodo_bk));
    bl->x = x + space;
    bl->y = y + space;
    bl->w = x + size - space;
    bl->h = y + size - space;
    bl->tipo = 2;
    bl->valor = 1;
    *rw_ctr += 1;

    return bl;
}

nodo_bk *block_type_3(float x, float y, float size, int space, int *rw_ctr)
{
    nodo_bk *bl;
    
    bl = malloc(sizeof(nodo_bk));
    bl->x = x + space;
    bl->y = y + space;
    bl->w = x + size - space;
    bl->h = y + size - space;
    bl->tipo = 3;
    bl->valor = 1;
    *rw_ctr += 1;

    return bl;
}

void block_insert_row(matrix_bl *m, float x, float y, int points)
{
    int j;

    for (j=0; j < COLS_B; j++)
        m->bl[0][j] = block_make(x + m->size*j, y, m->size, m->space, points, m->rw_c);

    if (points > 1 && m->rw_c->n_balls[0] == 0 && aleat(1, 100) <= PROB_BALL)
    {
        j = aleat(0, COLS_B);
        while (m->bl[0][j] != NULL && m->bl[0][j]->tipo == 2)
            j = aleat(0, COLS_B);
        
        if (m->bl[0][j] != NULL && m->bl[0][j]->tipo == 1)
            m->rw_c->n_blocks[0]--;
        
        block_destroy(&m->bl[0][j]);
        m->bl[0][j] = block_type_3(x + m->size*j, y, m->size, m->space + m->size*0.2, &m->rw_c->n_balls[0]);
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
                m->bl[i-1][j]->h += m->size;
                m->bl[i-1][j]->cy += m->size;
            }
            m->bl[i][j] = m->bl[i-1][j];
        }
        m->rw_c->n_coins[i] = m->rw_c->n_coins[i-1];
        m->rw_c->n_balls[i] = m->rw_c->n_balls[i-1];
        m->rw_c->n_blocks[i] = m->rw_c->n_blocks[i-1];
    }

    for (i=0; i < COLS_B; i++)
        m->bl[0][i] = NULL;
    m->rw_c->n_blocks[0] = 0;
    m->rw_c->n_coins[0] = 0;
    m->rw_c->n_balls[0] = 0;

    return 1;
}

int block_collide_bottom(matrix_bl *m, nodo_b *b, float raio, int *nb, int *coins)
{
    nodo_bk *bl;
    float dif, y;
    int c, r;

    dif = raio + m->space;

    if ((bl = block_get_by_quadrant(m, b->cx, b->cy - m->size/2, &r, &c)))
    {
        if (b->cx >= bl->x && b->cx <= bl->w && b->cy - dif <= bl->h)
        {
            y = bl->h + dif + 0.1;            
            if (block_resolve_collide(&m->bl[r][c], nb, coins, m->rw_c, r) == 1)
            {
                b->cy = y;
                b->sy = -b->sy;
            }
            return 1;
        }
    }

    return 0;
}

int block_collide_left(matrix_bl *m, nodo_b *b, float raio, int *nb, int *coins)
{
    nodo_bk *bl;
    float dif, x;
    int c, r;

    dif = raio + m->space*2;

    if ((bl = block_get_by_quadrant(m, b->cx + m->size, b->cy, &r, &c)))
    {
        if (b->cy >= bl->y && b->cy <= bl->h && b->cx + dif >= bl->x)
        {
            x = bl->x - dif - 0.1;
            if (block_resolve_collide(&m->bl[r][c], nb, coins, m->rw_c, r) == 1)
            {
                b->cx = x;
                b->sx = -b->sx;
            }
            return 1;
        }
    } 

    return 0;
}

int block_collide_top(matrix_bl *m, nodo_b *b, float raio, int *nb, int *coins)
{
    nodo_bk *bl;
    float dif, y;
    int c, r;
    
    dif = (raio + m->space);

    if ((bl = block_get_by_quadrant(m, b->cx, b->cy + m->size/2, &r, &c)))
    {
        if (b->cx >= bl->x && b->cx <= bl->w && b->cy + dif >= bl->y)
        {
            y = bl->y - dif - 0.1;
            if (block_resolve_collide(&m->bl[r][c], nb, coins, m->rw_c, r) == 1)
            {
                b->cy = y;
                b->sy = -b->sy;
            }
            return 1;
        }
    } 

    return 0;
}

int block_collide_right(matrix_bl *m, nodo_b *b, float raio, int *nb, int *coins)
{
    nodo_bk *bl;
    float dif, x;
    int c, r;

    dif = raio + m->space*2;

    if ((bl = block_get_by_quadrant(m, b->cx - m->size/2, b->cy, &r, &c)))
    {
        if (b->cy >= bl->y && b->cy <= bl->h && b->cx - dif <= bl->w)
        {
            x = bl->w + dif + 0.1;
            if (block_resolve_collide(&m->bl[r][c], nb, coins, m->rw_c, r) == 1)
            {
                b->cx = x;
                b->sx = -b->sx;
            }    
            return 1;
        }
    } 

    return 0;
}

int block_resolve_collide(nodo_bk **bl, int *new_balls, int *coins, row_control *r, int i)
{
    int x = (*bl)->tipo;

    switch (x)
    {
    case 1:
        (*bl)->valor--;
        if ((*bl)->valor == 0)
        {
            block_destroy(bl);
            r->n_blocks[i]--;
        }
        break;

    case 2:
        block_destroy(bl);
        r->n_coins[i]--;
        *coins += 1;
        break;

    case 3:
        block_destroy(bl);
        r->n_balls[i]--;
        *new_balls += 1;
        break;
    }

    return x;
}

nodo_bk *block_get_by_quadrant(matrix_bl *m, float x, float y, int *i, int *j)
{
    *i = (int)((y-87) / (m->size));
    *j = (int)((x-7) / (m->size));

    if (*i < 0 || *i >= ROWS_B || *j < 0 || *j >= COLS_B)
        return NULL;
    
    return m->bl[*i][*j];
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

    free((*m)->rw_c);
    (*m)->rw_c = NULL;

    free(*m);
    *m = NULL;
}