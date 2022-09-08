#include "sources/engine.h"
#include "sources/states.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

int main()
{
    engine_t *e;
    states st = MENU;

    srand(time(NULL));

    e = start_engine();
    
    while(st != END)
    {
        switch (st)
        {
        case START: /*state_start()*/; break;
        case MENU: state_menu(e, &st); break;
        case GAME: state_game(e, &st); break;
        default: break;
        }
    }

    end_engine(&e);

    return 0;
}