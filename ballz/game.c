#include "game.h"
#include <stdlib.h>

gb_t *game_make_box_p(float scr_w, float scr_h)
{
    gb_t *g;
    
    g = malloc(sizeof(gb_t));
    if (!g)
        return NULL;

    g->x = 0;
    g->y = scr_h * 0.1;
    g->w = scr_w;
    g->h = scr_h - g->y;

    return g;
}

void game_destroy(gb_t **g)
{
    free(*g);
    *g = NULL;
}