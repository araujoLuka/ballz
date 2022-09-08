#ifndef ENGINE_H_
#define ENGINE_H_

#include <allegro5/allegro.h>
#define SCR_W 480
#define SCR_H 800

typedef struct AL_ATRIBUTES
{
    ALLEGRO_DISPLAY *disp;
    ALLEGRO_EVENT event;
    ALLEGRO_EVENT_QUEUE *queue;
    ALLEGRO_BITMAP *cursor;
    ALLEGRO_TIMER *timer;
    struct GAME_FONTS *fonts;
    ALLEGRO_MOUSE_STATE msestate;
    ALLEGRO_KEYBOARD_STATE kbdstate;
} engine_t;

typedef struct GAME_FONTS
{
    void *f18;
    void *f22;
    void *f26;
} ALLEGRO_FONT_STRUCT;

engine_t *start_engine();

int start_allegro();

void *load_fonts();

void destroy_fonts(void **f);

void draw_game(engine_t *e, void *b, void *g, float dx, float dy, int pt);

engine_t *end_engine(engine_t* e);

#endif