#ifndef BLOCK_H_
#define BLOCK_H_
#define ROWS_B 10
#define COLS_B 8

typedef struct block
{
    float x, y, w, h, cx, cy;
    int tipo;
    int valor;
} block_t;

typedef struct matrix_row_control
{
    int n_blocks[ROWS_B];
    int n_coins[ROWS_B];
    int n_balls[ROWS_B];
} rwc_t;

typedef struct block_matrix
{
    block_t *bl[ROWS_B][COLS_B];
    rwc_t *rw_c;
    int size;
    float space;
} matrix_bl;

matrix_bl *block_matrix_make(float g_w);

rwc_t *block_rwc_make();

block_t *block_make(float x, float y, float size, int space, int points, rwc_t *r);

block_t *block_type_1(float x, float y, float size, int space, int points, int *rw_ctr);

block_t *block_type_2(float x, float y, float size, int space, int *rw_ctr);

block_t *block_type_3(float x, float y, float size, int space, int *rw_ctr);

void block_insert_row(matrix_bl *m, float x, float y, int points);

int block_move(matrix_bl *m);

block_t *block_get_by_quadrant(matrix_bl *m, float y, float x);

void block_destroy(matrix_bl **b);

#endif