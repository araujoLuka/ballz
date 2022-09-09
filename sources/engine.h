#ifndef ENGINE_H_
#define ENGINE_H_

#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>

#define SCR_W 480
#define SCR_H 800

typedef struct AL_ATRIBUTES engine_t;

#include "ball.h"
#include "block.h"
#include "object.h"

typedef struct GAME_FONTS
{
    ALLEGRO_FONT *f18;
    ALLEGRO_FONT *f22;
    ALLEGRO_FONT *f26;
    ALLEGRO_FONT *f30;
    ALLEGRO_FONT *f40;
    ALLEGRO_FONT *f60;
} ALLEGRO_FONT_STRUCT;

struct AL_ATRIBUTES
{
    ALLEGRO_DISPLAY *disp;
    ALLEGRO_EVENT event;
    ALLEGRO_EVENT_QUEUE *queue;
    ALLEGRO_BITMAP *cursor;
    ALLEGRO_TIMER *timer;
    ALLEGRO_FONT_STRUCT *fonts;
    ALLEGRO_MOUSE_STATE msestate;
    ALLEGRO_KEYBOARD_STATE kbdstate;
};

engine_t *start_engine();

int start_allegro();

void *load_fonts();

void destroy_fonts(ALLEGRO_FONT_STRUCT **f);

ALLEGRO_BITMAP *load_bitmap_at_size(const char *filename, int w, int h);

void end_engine(engine_t **e);

#endif