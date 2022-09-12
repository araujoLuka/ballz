#ifndef ENGINE_H_
#define ENGINE_H_

#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>

#define SCR_W 480
#define SCR_H 800

typedef struct AL_ATRIBUTES engine_t;
typedef struct ENGINE_FONTS ALLEGRO_FONTS;
typedef struct ENGINE_SOUNDS ALLEGRO_SOUNDS;

#include "ball.h"
#include "block.h"
#include "object.h"

struct ENGINE_FONTS
{
    ALLEGRO_FONT *l10;
    ALLEGRO_FONT *r18;
    ALLEGRO_FONT *r22;
    ALLEGRO_FONT *r26;
    ALLEGRO_FONT *r30;
    ALLEGRO_FONT *b22;
    ALLEGRO_FONT *r40;
    ALLEGRO_FONT *l60;
};

struct ENGINE_SOUNDS
{
    ALLEGRO_SAMPLE *bgrd;
    ALLEGRO_SAMPLE *coin;
    ALLEGRO_SAMPLE *ball_up;
    ALLEGRO_SAMPLE *hit_wall;
    ALLEGRO_SAMPLE *hit_block;
};

struct AL_ATRIBUTES
{
    ALLEGRO_DISPLAY *disp;
    ALLEGRO_EVENT event;
    ALLEGRO_EVENT_QUEUE *queue;
    ALLEGRO_BITMAP *cursor;
    ALLEGRO_TIMER *timer;
    ALLEGRO_FONTS *fonts;
    ALLEGRO_SOUNDS *sounds;
    ALLEGRO_MOUSE_STATE msestate;
    ALLEGRO_KEYBOARD_STATE kbdstate;
    float curr_t, last_t, delta_t;
};

engine_t *start_engine();

int start_allegro();

ALLEGRO_BITMAP *load_bitmap_at_size(const char *filename, int w, int h);

ALLEGRO_FONTS *load_fonts();

ALLEGRO_SOUNDS *load_sounds();

void draw_text(ALLEGRO_FONT *font, ALLEGRO_COLOR color, float x, float y, int flags,const char *str);

void draw_num(ALLEGRO_FONT *font, ALLEGRO_COLOR color, float x, float y, int flags, int num);

void draw_button(ALLEGRO_FONTS *fonts, ALLEGRO_COLOR color, box_t pos, const char *str, vec_t ms);

void draw_icon(const char *filename, ALLEGRO_COLOR color, circ_t pos, vec_t ms);

void destroy_fonts(ALLEGRO_FONTS **f);

void destroy_sounds(ALLEGRO_SOUNDS **s);

void end_engine(engine_t **e);

#endif