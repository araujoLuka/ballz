#include "block.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#define PROB_BLOCK 50
#define PROB_BALL 30
#define PROB_GOLD 15

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

    m->space = 3;
    m->size = g_w / (COLS_B);

    return m;
}

rwc_t *block_rwc_make()
{
    rwc_t *r;

    if (!(r = malloc(sizeof(rwc_t))))
        return NULL;

    for (int i=0; i < ROWS_B; i++)
    {
        r->n_blocks[i] = 0;
        r->n_coins[i] = 0;
        r->n_balls[i] = 0;
    }

    return r;
}

block_t *block_make(float x, float y, float size, int space, int points, rwc_t *r)
{
    int prob, bonus_b;
    prob = aleat(0, 100);
    bonus_b = 0;

    if (r->n_blocks[0] < 2)
        bonus_b = 10;
    
    if (points > 1 && r->n_balls[0] == 0 && prob <= PROB_BALL)
        return block_type_3(x, y, size, space*7, &r->n_balls[0]);
    
    if (points > 1 && r->n_coins[0] == 0 && prob <= PROB_GOLD)
        return block_type_2(x, y, size, space*6, &r->n_coins[0]);
    
    if (r->n_blocks[0] <= 6 && prob <= PROB_BLOCK + bonus_b)
        return block_type_1(x, y, size, space, points, &r->n_blocks[0]);

    return NULL;
}

block_t *block_type_1(float x, float y, float size, int space, int points, int *rw_ctr)
{
    block_t *bl;
    
    bl = malloc(sizeof(block_t));
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

block_t *block_type_2(float x, float y, float size, int space, int *rw_ctr)
{
    block_t *bl;
    
    bl = malloc(sizeof(block_t));
    bl->x = x + space;
    bl->y = y + space;
    bl->w = x + size - space;
    bl->h = y + size - space;
    bl->tipo = 2;
    bl->valor = 1;
    *rw_ctr += 1;

    return bl;
}

block_t *block_type_3(float x, float y, float size, int space, int *rw_ctr)
{
    block_t *bl;
    
    bl = malloc(sizeof(block_t));
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
    for (int j=0; j < COLS_B; j++)
        m->bl[0][j] = block_make(x + m->size*j, y, m->size, m->space, points, m->rw_c);
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
                m->rw_c->n_balls[i] = m->rw_c->n_balls[i-1];
                m->rw_c->n_blocks[i] = m->rw_c->n_balls[i-1];
            }
            m->bl[i][j] = m->bl[i-1][j];
        }
    }
    for (i=0; i < COLS_B; i++)
        m->bl[0][i] = NULL;
    m->rw_c->n_blocks[0] = 0;
    m->rw_c->n_coins[0] = 0;
    m->rw_c->n_balls[0] = 0;

    if (m->rw_c->n_blocks[ROWS_B-1] > 0)
        return 0;

    return 1;
}

block_t *block_get_by_quadrant(matrix_bl *m, float x, float y)
{
    return NULL;
}

void block_destroy(matrix_bl **m)
{
    for (int i=0; i < ROWS_B; i++)
    {
        for(int j=0; j < COLS_B; j++)
        {
            free((*m)->bl[i][j]);
            (*m)->bl[i][j] = NULL;
        }
    }
    fprintf(stderr, "Aqui\n");

    free((*m)->rw_c);
    (*m)->rw_c = NULL;
    fprintf(stderr, "Aqui\n");

    free(*m);
    *m = NULL;
    fprintf(stderr, "Aqui\n");
}