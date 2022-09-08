#ifndef BLOCK_H_
#define BLOCK_H_
#define ROWS_B 10
#define COLS_B 8

typedef struct block nodo_bk;
typedef struct matrix_row_control row_control;
typedef struct block_matrix matrix_bl;

#include "ball.h"

struct block
{
    float x, y, w, h, cx, cy;
    int tipo;
    int valor;
};

struct matrix_row_control
{
    int n_blocks[ROWS_B];
    int n_coins[ROWS_B];
    int n_balls[ROWS_B];
};

struct block_matrix
{
    nodo_bk *bl[ROWS_B][COLS_B];
    row_control *rw_c;
    int size;
    float space;
};

matrix_bl *block_matrix_make(float g_w);

row_control *block_rwc_make();

nodo_bk *block_make(float x, float y, float size, int space, int points, row_control *r);

nodo_bk *block_type_1(float x, float y, float size, int space, int points, int *rw_ctr);

nodo_bk *block_type_2(float x, float y, float size, int space, int *rw_ctr);

nodo_bk *block_type_3(float x, float y, float size, int space, int *rw_ctr);

void block_insert_row(matrix_bl *m, float x, float y, int points);

int block_move(matrix_bl *m);

int block_collide_bottom(matrix_bl *m, nodo_b *b, float raio, int *nb, int *coins);

int block_collide_left(matrix_bl *m, nodo_b *b, float raio, int *nb, int *coins);

int block_collide_top(matrix_bl *m, nodo_b *b, float raio, int *nb, int *coins);

int block_collide_right(matrix_bl *m, nodo_b *b, float raio, int *nb, int *coins);

int block_resolve_collide(nodo_bk **bl, int *new_balls, int *coins, row_control *r, int i);

nodo_bk *block_get_by_quadrant(matrix_bl *m, float x, float y, int *i, int *j);

void block_destroy(nodo_bk **bl);

void block_matrix_destroy(matrix_bl **b);

#endif