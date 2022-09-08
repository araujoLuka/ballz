#include "engine.h"
#include "libball.h"
#include "game.h"
#include <stdlib.h>
#include <stdio.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#define FPS 60

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

    if (!f->f18 || !f->f22 || !f->f26)
    {
        destroy_fonts((void**)&f);
        fprintf(stderr, "Falha ao carregar fontes\n");
        return NULL;
    }

    return f;
}

void destroy_fonts(void **f)
{
    al_destroy_font(((ALLEGRO_FONT_STRUCT*)*f)->f18);
    al_destroy_font(((ALLEGRO_FONT_STRUCT*)*f)->f22);
    al_destroy_font(((ALLEGRO_FONT_STRUCT*)*f)->f26);
    free(*f);
    *f = NULL;
}

void draw_game(engine_t *e, void *ptr_b, void *ptr_g, float dx, float dy, int pt)
{
    list_b *b = ptr_b;
    gb_t *g = ptr_g;
    ball_t *aux;
    float aim_x, aim_y;
    aim_x = b->ini->x + b->raio;
    aim_y = b->ini->y + b->raio;
    aux = b->ini;

    al_clear_to_color(al_map_rgb(30, 30, 30));
    al_draw_filled_rectangle(g->x, g->y, g->w, g->h, al_map_rgb(20, 20, 20));
    al_draw_text(e->fonts->f26, al_map_rgb(230,230,230), SCR_W/4, g->y * 0.3, 1, "BALLZ");
    al_draw_textf(e->fonts->f26, al_map_rgb(230,230,230), SCR_W - SCR_W/4, g->y * 0.3, 1, "%d", pt);
    if (b->launch)
    {
        for (int i=0; i < b->tam; i++)
        {
            al_draw_bitmap(b->img, aux->x, aux->y, 0);
            aux = aux->next;
        }
    }
    else
    {
        al_draw_bitmap(b->img, aux->x, aux->y, 0);
        if (dy < -20)
            al_draw_line(aim_x, aim_y, aim_x + dx, aim_y + dy, al_map_rgb(255, 255, 255), 2);
    }
    al_draw_bitmap(e->cursor, e->msestate.x, e->msestate.y, 0);
    al_flip_display();
}

engine_t *end_engine(engine_t *e)
{
    al_destroy_display(e->disp);
    al_destroy_timer(e->timer);
    al_destroy_event_queue(e->queue);
    al_destroy_bitmap(e->cursor);
    destroy_fonts((void**)&e->fonts);

    return NULL;
}
