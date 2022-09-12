#include "sources/engine.h"
#include "sources/states.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

int main()
{
    engine_t *e;
    u_data *d;
    states st = START;

    srand(time(NULL));

    e = start_engine();
    al_play_sample(e->sounds->bgrd, 0.3, 0, 1, ALLEGRO_PLAYMODE_LOOP, NULL);
    
    while(st != END)
    {
        switch (st)
        {
            case START: state_start(e, &d, &st); break;
            
            case MENU: state_menu(e, d, &st); break;

            case HELP: state_help(e, &st); break;

            case GAME: state_game(e, d, &st); break;
            
            default: break;
        }
    }

    end_engine(&e);

    return 0;
}