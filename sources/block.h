#ifndef BLOCK_H_
#define BLOCK_H_
#define ROWS_B 10
#define COLS_B 8

typedef enum { BLOCK, BALL, COIN } type_b;

typedef struct block nodo_bk;
typedef struct matrix_row_control row_control;
typedef struct block_matrix matrix_bl;

#include "ball.h"
#include "engine.h"

struct block
{
    float x, y, x2, y2, cx, cy;
    type_b t;
    int value, hit;
};

struct matrix_row_control
{
    int n_blocks;
    bool have_coin;
    bool have_ball;
};

struct block_matrix
{
    nodo_bk *bl[ROWS_B][COLS_B];
    row_control rows[ROWS_B];
    int size;
    float space;
};

matrix_bl *block_matrix_make(float g_w);

void block_rwc_make(row_control *r);

nodo_bk *block_make(float x, float y, float size, int space, int points, row_control *r);

nodo_bk *block_type_block(float x, float y, float size, int space, int points, int *n_blocks);

nodo_bk *block_type_coin(float x, float y, float size, int space, bool *h_coin);

nodo_bk *block_type_ball(float x, float y, float size, int space, bool *h_ball);

void block_insert_row(matrix_bl *m, float x, float y, int points);

int block_move(matrix_bl *m);

// Tratamento para colisao com o bloco, retorna o tipo do bloco
type_b block_resolve_collide(nodo_bk **bl, int *new_balls, int *coins, row_control *r);

int block_collide_bottom(matrix_bl *m, nodo_b *b, float raio, int *nb, int *coins);

int block_collide_left(matrix_bl *m, nodo_b *b, float raio, int *nb, int *coins);

int block_collide_top(matrix_bl *m, nodo_b *b, float raio, int *nb, int *coins);

int block_collide_right(matrix_bl *m, nodo_b *b, float raio, int *nb, int *coins);

nodo_bk *block_get_by_quadrant(matrix_bl *m, float x, float y, int *i, int *j);

void block_draw(engine_t *e, matrix_bl *m, int anim);

void block_destroy(nodo_bk **bl);

void block_matrix_destroy(matrix_bl **b);

#endif