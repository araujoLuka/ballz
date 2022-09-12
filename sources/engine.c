#include "engine.h"
#include <stdlib.h>
#include <stdio.h>
#define FPS 60

engine_t *start_engine()
{
    engine_t *e;

    if (!(e = malloc(sizeof(engine_t))))
        return NULL;

    if (!start_allegro())
    {
        free(e);
        return NULL;
    }

    if (!(e->timer = al_create_timer(1.0 / (double)FPS)))
    {
        free(e);
        fprintf(stderr, "Falha ao criar timer\n");
        return NULL;
    }

    if (!(e->queue = al_create_event_queue()))
    {
        free(e);
        fprintf(stderr, "Falha ao criar fila de eventos\n");
        return NULL;
    }
    
    if (!(e->disp = al_create_display(SCR_W, SCR_H))){
        free(e);
        fprintf(stderr, "Falha ao criar display.\n");
        return NULL;
    }

    al_hide_mouse_cursor(e->disp);
    if (!(e->cursor = load_bitmap_at_size("resources/cursor/cursor.png", 30, 30)))
    {
        free(e);
        fprintf(stderr, "Falha ao criar cursor.\n");
        return NULL;
    }

    al_reserve_samples(16);
    
    e->fonts = load_fonts();
    e->sounds = load_sounds();
    e->curr_t = 0;
    e->last_t = 0;
    e->delta_t = 0;

    al_register_event_source(e->queue, al_get_keyboard_event_source());
    al_register_event_source(e->queue, al_get_display_event_source(e->disp));
    al_register_event_source(e->queue, al_get_timer_event_source(e->timer));
    
    return e;
}

int start_allegro()
{
    if (!al_init()){
        fprintf(stderr, "Falha ao iniciar Allegro\n");
        return 0;
    }

    if (!al_init_primitives_addon())
    {
        fprintf(stderr, "Falha ao iniciar primitives addon\n");
        return 0;
    }

    if (!al_init_font_addon())
    {
        fprintf(stderr, "Falha ao iniciar font addon\n");
        return 0;
    }

    if (!al_init_ttf_addon())
    {
        fprintf(stderr, "Falha ao iniciar ttf addon\n");
        return 0;
    }

    if (!al_install_keyboard())
    {
        fprintf(stderr, "Falha ao iniciar controle de keyboard\n");
        return 0;
    }

    if (!al_install_mouse())
    {
        fprintf(stderr, "Falha ao iniciar controle de mouse\n");
        return 0;
    }

    if (!al_init_image_addon())
    {
        fprintf(stderr, "Falha ao iniciar image addon\n");
        return 0;
    }

    if (!al_install_audio())
    {
        fprintf(stderr, "Falha ao iniciar controle de audio\n");
        return 0;
    }

    if (!al_init_acodec_addon())
    {
        fprintf(stderr, "Falha ao iniciar audio codec addon\n");
        return 0;
    }

    return 1;
}

ALLEGRO_BITMAP *load_bitmap_at_size(const char *filename, int w, int h)
{
  ALLEGRO_BITMAP *resized_bmp, *loaded_bmp, *prev_target;

  // 1. create a temporary bitmap of size we want
  resized_bmp = al_create_bitmap(w, h);
  if (!resized_bmp) return NULL;

  // 2. load the bitmap at the original size
  loaded_bmp = al_load_bitmap(filename);
  if (!loaded_bmp)
  {
     al_destroy_bitmap(resized_bmp);
     return NULL;
  }

  // 3. set the target bitmap to the resized bmp
  prev_target = al_get_target_bitmap();
  al_set_target_bitmap(resized_bmp);

  // 4. copy the loaded bitmap to the resized bmp
  al_draw_scaled_bitmap(loaded_bmp,
     0, 0,                                // source origin
     al_get_bitmap_width(loaded_bmp),     // source width
     al_get_bitmap_height(loaded_bmp),    // source height
     0, 0,                                // target origin
     w, h,                                // target dimensions
     0                                    // flags
  );

  // 5. restore the previous target and clean up
  al_set_target_bitmap(prev_target);
  al_destroy_bitmap(loaded_bmp);

  return resized_bmp;      
}

ALLEGRO_FONTS *load_fonts()
{
    ALLEGRO_FONTS *f;

    if (!(f = malloc(sizeof(ALLEGRO_FONTS))))
    {
        fprintf(stderr, "Falha ao alocar memoria para fontes\n");
        return NULL;
    }

    f->l10 = al_load_font("resources/fonts/light.otf", 10, 0);
    f->r18 = al_load_font("resources/fonts/regular.otf", 18, 0);
    f->r22 = al_load_font("resources/fonts/regular.otf", 22, 0);
    f->r26 = al_load_font("resources/fonts/regular.otf", 26, 0);
    f->r30 = al_load_font("resources/fonts/regular.otf", 30, 0);
    f->r40 = al_load_font("resources/fonts/regular.otf", 40, 0);
    f->b22 = al_load_font("resources/fonts/bold.otf", 22, 0);
    f->l60 = al_load_font("resources/fonts/light.otf", 60, 0);

    if (!f->l10 || !f->r18 || !f->r22 || !f->r26 || !f->r30 || !f->b22 || !f->r40 || !f->l60)
    {
        destroy_fonts(&f);
        fprintf(stderr, "Falha ao carregar fontes\n");
        return NULL;
    }

    return f;
}

ALLEGRO_SOUNDS *load_sounds()
{
    ALLEGRO_SOUNDS *s;

    if (!(s = malloc(sizeof(ALLEGRO_SOUNDS))))
    {
        fprintf(stderr, "Falha ao alocar memoria para audios\n");
        return NULL;
    }

    s->bgrd = al_load_sample("resources/sounds/background.wav");
    s->coin = al_load_sample("resources/sounds/coin.wav");
    s->ball_up = al_load_sample("resources/sounds/ballup.wav");
    s->hit_wall = al_load_sample("resources/sounds/hit1.wav");
    s->hit_block = al_load_sample("resources/sounds/hit2.wav");

    if (!s->bgrd || !s->coin || !s->ball_up || !s->hit_wall || !s->hit_block)
    {
        destroy_sounds(&s);
        fprintf(stderr, "Falha ao carregar audios\n");
        return NULL;
    }

    return s;
}

void draw_text(ALLEGRO_FONT *font, ALLEGRO_COLOR color, float x, float y, int flags, const char *str)
{
    float font_offset;

    font_offset = al_get_font_line_height(font)/2;
    al_draw_text(font, al_map_rgb(255, 255, 255), x, y - font_offset, flags, str);
}

void draw_num(ALLEGRO_FONT *font, ALLEGRO_COLOR color, float x, float y, int flags, int num)
{
    float font_offset;

    font_offset = al_get_font_line_height(font)/2;
    al_draw_textf(font, color, x, y - font_offset, flags, "%d", num);
}

void draw_button(ALLEGRO_FONTS *fonts, ALLEGRO_COLOR color, box_t pos, const char *str, vec_t ms)
{
    ALLEGRO_FONT *f;
    ALLEGRO_COLOR sel_color;
    float font_offset;
    vec_t text_pos;

    f = fonts->r22;
    font_offset = al_get_font_line_height(f)/2;
    text_pos = vector_make(pos.x1 + pos.w/2, pos.y1 + pos.h/2 - font_offset);
    sel_color = al_map_rgb_f(color.r*0.8, color.g*0.8, color.b*0.8);

    if (box_inside(ms, pos))
        al_draw_filled_rounded_rectangle(pos.x1, pos.y1, pos.x2, pos.y2, 30, 30, sel_color);
    else
        al_draw_filled_rounded_rectangle(pos.x1, pos.y1, pos.x2, pos.y2, 30, 30, color);
        
    al_draw_text(f, al_map_rgb(255, 255, 255), text_pos.x, text_pos.y, -1, str);
}

void draw_icon(const char *filename, ALLEGRO_COLOR color, circ_t pos, vec_t ms)
{
    ALLEGRO_BITMAP *img;
    ALLEGRO_COLOR sel_color;
    float img_offset;

    img = load_bitmap_at_size(filename, pos.radius*1.2, pos.radius*1.2);

    sel_color = al_map_rgb_f(color.r*0.8, color.g*0.8, color.b*0.8);

    if (circle_inside(ms, pos))
        al_draw_filled_circle(pos.x, pos.y, pos.radius, sel_color);
    else
        al_draw_filled_circle(pos.x, pos.y, pos.radius, color);
    
    if (img != NULL)
    {
        img_offset = al_get_bitmap_width(img)/2;
        al_draw_bitmap(img, pos.x-img_offset, pos.y-img_offset, 0);
    }
}

void destroy_fonts(ALLEGRO_FONTS **f)
{
    al_destroy_font((*f)->r18);
    al_destroy_font((*f)->r22);
    al_destroy_font((*f)->r26);
    al_destroy_font((*f)->r30);
    al_destroy_font((*f)->r40);
    al_destroy_font((*f)->l60);
    free(*f);
    *f = NULL;
}

void destroy_sounds(ALLEGRO_SOUNDS **s)
{
    al_destroy_sample((*s)->bgrd);
    al_destroy_sample((*s)->coin);
    *s = NULL;
}

void end_engine(engine_t **e)
{
    al_destroy_display((*e)->disp);
    al_destroy_timer((*e)->timer);
    al_destroy_event_queue((*e)->queue);
    al_destroy_bitmap((*e)->cursor);
    destroy_fonts(&(*e)->fonts);
    free(*e);
    *e = NULL;
}