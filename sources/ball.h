/* BALLZ TO LINUX - BALL LIBRARY
 *
 * Author: Lucas Araujo
 * Git: https://github.com/araujoLuka/ballz/blob/main/sources/ball.h
 * 
 * Describe: Library to generate and control balls
*/

#ifndef BALL_H_
#define BALL_H_

typedef struct balls ball_t;
typedef struct nodo_ball nodo_b;

#include "engine.h"
#include "block.h"
#include "object.h"

// Nodo bolinha da lista com posição e velocidade
struct nodo_ball
{
    float cx, cy, sx, sy;
    int move;
    nodo_b *next;
};

// Estrutura com atributos e inicio da lista de bolinhas
struct balls
{
    ALLEGRO_BITMAP *img;    // bitmap para bolinhas
    float radius;       // raio das bolinhas
    nodo_b *ini;        // inicio da lista de bolinhas
    nodo_b *first;      // primeira bolinha a tocar o chao (depois de um lançamento)
    int tam, n;
    int vel;
    int l_ctr;          // contador de lançamento
    float l_tmr;        // momento em que foi lançado
    bool launch;
};

// Cria a lista de bolinhas
ball_t *ball_list_make(float disp_w, float disp_h, int sel);

// Cria uma bolinha
nodo_b *ball_make(double x, double y);

// Insere 'q' bolinhas na lista
int ball_insert(ball_t *b, int q);

// Realiza o lançamento das bolinhas
void ball_launch(ball_t *b, vec_t *d, float scale);

// Desenha as bolinhas na tela
void ball_draw(ball_t *b);

// Trata colisao com a parede esquerda da area de jogo
int ball_collide_left(nodo_b *b, float r, int lim, ALLEGRO_SAMPLE *sound);

// Trata colisao com a parede direita da area de jogo
int ball_collide_right(nodo_b *b, float r, int lim, ALLEGRO_SAMPLE *sound);

// Trata colisao com o teto da area de jogo
int ball_collide_top(nodo_b *b, float r, int lim, ALLEGRO_SAMPLE *sound);

// Trata colisao com o chao da area de jogo
int ball_collide_bottom(nodo_b *b, float r, int lim);

// Trata colisao com blocos
int ball_collide_block(nodo_b *b, matrix_bl *m, float radius, int *nb, int *coins, ALLEGRO_SOUNDS *s);

// Libera memoria da lista de bolinhas
void ball_list_destroy(ball_t **b);

#endif