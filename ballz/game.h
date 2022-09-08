#ifndef GAME_H_
#define GAME_H_

typedef struct gameBox
{
    float x, y;
    float w, h;
}gb_t;

gb_t *game_make_box_p(float scr_w, float scr_h);

void game_destroy(gb_t **g);

#endif