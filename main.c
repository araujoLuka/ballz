#include "ball.h"
#include "game.h"
#include "engine.h"
#include "block.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

typedef enum { START, MENU, GAME } states;

int main()
{
    engine_t *e;
    states st = GAME;

    srand(time(NULL));

    e = start_engine();
    
    while(1)
    {
        switch (st)
        {
        case START: state_start(); break;
        case MENU: state_menu(); break;
        case GAME: state_game(e); break;
        default: break;
        }
    }

    end_engine(&e);

    return 0;
}