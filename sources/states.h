#ifndef STATES_H_
#define STATES_H_

#include "engine.h"
#include "data.h"

typedef enum { START, MENU, HELP, GAME, END } states;

void state_start(engine_t *e, u_data **d, states *st);

void state_menu(engine_t *e, u_data *d, states *st);

void state_help(engine_t *e, states *st);

void state_game(engine_t *e, u_data *d, states *st);

#endif