/* BALLZ TO LINUX - STATES LIBRARY
 *
 * Author: Lucas Araujo
 * Git: https://github.com/araujoLuka/ballz/blob/main/sources/states.h
 * 
 * Describe: Library responsible for control program states
*/

#ifndef STATES_H_
#define STATES_H_

#include "engine.h"
#include "data.h"

// Estados do programa
typedef enum { START, MENU, HELP, GAME, END } states;

// Estado START - carrega os dados do usuario e direciona para o menu
void state_start(engine_t *e, u_data **d, states *st);

// Estado MENU - exibe a tela de menu do jogo
void state_menu(engine_t *e, u_data *d, states *st);

// Estado HELP - exibe informações de como jogar
void state_help(engine_t *e, states *st);

// Estado GAME - jogar uma partida
void state_game(engine_t *e, u_data *d, states *st);

#endif