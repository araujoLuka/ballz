/* BALLZ TO LINUX - GAME LIBRARY
 *
 * Author: Lucas Araujo
 * Git: https://github.com/araujoLuka/ballz/blob/main/sources/game.h
 * 
 * Describe: Library responsible for storing structures and functions used to play game
*/

#ifndef GAME_H_
#define GAME_H_

#include "engine.h"
#include "object.h"
#include "ball.h"
#include "block.h"
#include "data.h"

// Estados da partida
typedef enum { ANIM, AIM, LAUNCH, RESET, OVER, PAUSE, RESTART, STOP } g_states;

// Atributos principais da partida
typedef struct game
{
    matrix_bl *m;   // Matriz para os blocos
    ball_t *b;      // Lista de bolinhas
    box_t gm_box;   // Obejto representando area jogavel
    g_states gst;   // Estado atual do jogo
    u_data **d;     // Dados de usuario(moedas/score)
    int points, high, coins;
    float scale;
    bool paused;
} game_t;

// Cria a estrutura principal do jogo e estruturas secundarias
game_t *game_make(float scr_w, float scr_h, u_data **d);

// Carrega as informações iniciais do jogo (executado uma unica vez)
int game_start(engine_t *e, game_t **gs, u_data **d, vec_t *start, vec_t *delta);

// Atualiza as informações do jogo (executado a cada frame da partida)
int game_update(engine_t *e, game_t *gs, bool *mspress, vec_t *aim1, vec_t *aim2, int *nb, float *lt);

// Trata os eventos externos (mouse, teclado, timer, janela do jogo fechada)
g_states game_events(engine_t *e, g_states gst, g_states lst, float *scale);

// Estado de jogo ANIM (Animação) - desenha a transição entre niveis
void game_anim(engine_t *e, game_t *gs);

// Estado de jogo AIM (Mira) - calcula a mira da bolinha
void game_aim(engine_t *e, game_t *gs, bool *mspress, vec_t *start, vec_t *delta);

// Estado de jogo LAUNCH (Lançamento) - realiza o lançamento da lista de bolinhas
void game_launch(engine_t *e, game_t *gs, vec_t *v, float *lt, int *nb);

// Estado de jogo RESET (Fim de lançamento) - verifica se o jogo pode continuar
// Se sim continua a partida, senão direciona para o fim do jogo
void game_reset(engine_t *e, game_t *gs, vec_t *v, int *nb);

// Estado de jogo OVER (Fim de jogo) - registra os dados do usuario e desenha a tela de 'game-over'
void game_over(engine_t *e, game_t *gs);

// Estado de jogo PAUSE (Pausa) - Exibe a tela de pausa
void game_paused(engine_t *e, game_t *gs, g_states lst);

// Verifica se o lançamento de bolinhas terminou
int game_launch_end(engine_t *e, game_t *gs, vec_t *v, int *nb);

// Função de desenvolvedor - para teste de movimentação
void game_test(engine_t *e, game_t *gs, int *nb);

// Desenha os elementos principais do jogo
void game_draw(engine_t *e, game_t *gs, vec_t *vector, int anim);

// Desenha a mira calculada em AIM
void game_draw_aim(ball_t *b, float dx, float dy);

// Desenha a tela de pause
void game_draw_pause(engine_t *e, vec_t mouse_pos, box_t gme_b, box_t rsm_b, box_t rst_b, box_t menu_b);

// Desenha a tela de fim de jogo
void game_draw_over(engine_t *e, game_t *gs, vec_t mouse_pos, box_t rst_b, box_t mnu_b, bool new_best);

// Libera a memoria e o estado de jogo
void game_destroy(game_t **gs);

#endif