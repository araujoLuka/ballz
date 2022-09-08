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
    if (!(e->cursor = load_bitmap_at_size("resources/cursor.png", 30, 30)))
    {
        free(e);
        fprintf(stderr, "Falha ao criar cursor.\n");
        return NULL;
    }

    e->fonts = load_fonts();

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

    return 1;
}

void *load_fonts()
{
    ALLEGRO_FONT_STRUCT *f;

    if (!(f = malloc(sizeof(ALLEGRO_FONT_STRUCT))))
    {
        fprintf(stderr, "Falha ao alocar memoria para fontes\n");
        return NULL;
    }

    f->f18 = al_load_font("resources/font.otf", 18, 0);
    f->f22 = al_load_font("resources/font.otf", 22, 0);
    f->f26 = al_load_font("resources/font.otf", 26, 0);
    f->f30 = al_load_font("resources/font.otf", 30, 0);
    f->f40 = al_load_font("resources/font.otf", 40, 0);
    f->f60 = al_load_font("resources/font.otf", 60, 0);

    if (!f->f18 || !f->f22 || !f->f26 || !f->f30 || !f->f40 || !f->f60)
    {
        destroy_fonts(&f);
        fprintf(stderr, "Falha ao carregar fontes\n");
        return NULL;
    }

    return f;
}

void destroy_fonts(ALLEGRO_FONT_STRUCT **f)
{
    al_destroy_font((*f)->f18);
    al_destroy_font((*f)->f22);
    al_destroy_font((*f)->f26);
    al_destroy_font((*f)->f30);
    al_destroy_font((*f)->f40);
    al_destroy_font((*f)->f60);
    free(*f);
    *f = NULL;
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