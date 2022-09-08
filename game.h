#ifndef GAME_H_
#define GAME_H_

typedef struct gameBox
{
    float x, y;
    float w, h;
}gb_t;

gb_t *cria_gamebox(float scr_w, float scr_h);

#endif