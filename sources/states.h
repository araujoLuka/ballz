#ifndef STATES_H_
#define STATES_H_

#include "engine.h"

typedef enum { START, MENU, GAME, END } states;

void state_menu(engine_t *e, states *st);

void state_game(engine_t *e, states *st);

#endif