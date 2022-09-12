/* BALLZ TO LINUX - BLOCK LIBRARY
 *
 * Author: Lucas Araujo
 * Git: https://github.com/araujoLuka/ballz/blob/main/sources/block.h
 * 
 * Describe: Library to generate and control blocks
*/

#ifndef BLOCK_H_
#define BLOCK_H_

// Dimensões da matriz de blocos
#define ROWS_B 10
#define COLS_B 8

// Probabilidades dos tipos de bloco
#define PROB_BLOCK 75
#define PROB_BALL 100
#define PROB_COIN 10

// Tipos de blocos
typedef enum { NONE, BLOCK, BALL, COIN } type_b;

// Estrutura com a posição e atributos do bloco
typedef struct block nodo_bk;

// Estrutura para controlar a quantidade de blocos de cada linha da matriz
typedef struct matrix_row_control row_control;

// Estrutura que controla a matriz de blocos
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
    ALLEGRO_BITMAP *coin_img;
    int size;
    float space;
};

// Aloca memoria para uma matriz de blocos
matrix_bl *block_matrix_make(float g_w);

// Inicializa a estrutura de controle de linhas
void block_rwc_make(row_control *r);

// Calcula um valor aletorio em um intervalo (usado para calculo da probabilidade)
int prob (int ini, int fim);

// Define o tipo do bloco de acordo com uma probabilidade e cria (ou não) esse bloco
nodo_bk *block_make(float x, float y, float size, int space, int points, row_control *r);

// Cria um bloco padrão
nodo_bk *block_type_block(float x, float y, float size, int space, int points, int *n_blocks);

// Cria uma moeda
nodo_bk *block_type_coin(float x, float y, float size, int space, bool *h_coin);

// Cria um bonus de bolinha
nodo_bk *block_type_ball(float x, float y, float size, int space, bool *h_ball);

// Insere uma nova linha no inicio da matriz
void block_insert_row(matrix_bl *m, float x, float y, int points);

// Desce todas as linhas da matrix em uma unidade e atualiza a posição dos blocos
int block_move(matrix_bl *m);

// Tratamento para colisao com o bloco, retorna o tipo do bloco
type_b block_resolve_collide(nodo_bk **bl, int *new_balls, int *coins, row_control *r, ALLEGRO_SOUNDS *s);

// Verifica se existe colisão com o bloco
bool block_collision(nodo_bk *bl, nodo_b *b, float radius);

// Realiza a verificação de colisão embaixo do bloco
type_b block_collide_bottom(matrix_bl *m, nodo_b *b, float radius, int *nb, int *coins, ALLEGRO_SOUNDS *s);

// Realiza a verificação de colisão na esquerda do bloco
type_b block_collide_left(matrix_bl *m, nodo_b *b, float radius, int *nb, int *coins, ALLEGRO_SOUNDS *s);

// Realiza a verificação de colisão em cima do bloco
type_b block_collide_top(matrix_bl *m, nodo_b *b, float radius, int *nb, int *coins, ALLEGRO_SOUNDS *s);

// Realiza a verificação de colisão na direita do bloco
type_b block_collide_right(matrix_bl *m, nodo_b *b, float radius, int *nb, int *coins, ALLEGRO_SOUNDS *s);

// Retorna um bloco da matriz de acordo com uma posição especifica
nodo_bk *block_get_by_quadrant(matrix_bl *m, float x, float y, int *i, int *j);

// Desenha os blocos na tela
void block_draw(engine_t *e, matrix_bl *m, int anim);

// Libera memoria de um bloco
void block_destroy(nodo_bk **bl);

// Libera memoria da matriz de blocos
void block_matrix_destroy(matrix_bl **b);

#endif