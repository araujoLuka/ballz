#include "libball.h"
#include "game.h"
#include <stdlib.h>
#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>

#define NUM_BUTTONS 3
#define SCR_W 480
#define SCR_H 800

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

int main()
{
    ALLEGRO_DISPLAY *disp;
    ALLEGRO_EVENT event;
    ALLEGRO_EVENT_QUEUE *queue;
    ALLEGRO_BITMAP *cursor;
    ALLEGRO_TIMER *timer;
    ALLEGRO_FONT *font;
    ALLEGRO_MOUSE_STATE msestate;
    ALLEGRO_KEYBOARD_STATE kbdstate;
    ball_st *b;
    gb_t *g;
    bool done = false, mspress = false;
    float x0 = 0, y0 = 0;
    float delta_x, delta_y;
    float l_timer, n_timer;
    int points = 1;

    if (!al_init()){
        fprintf(stderr, "Falha ao iniciar Allegro.\n");
        return 1;
    }

    al_init_primitives_addon();
    al_init_font_addon();
    al_init_ttf_addon();
    al_install_keyboard();
    al_install_mouse();
    al_init_image_addon();

    timer = al_create_timer(1.0 / 60.0);
    queue = al_create_event_queue();
    font = al_load_font("resources/font.otf", 26, 0);

    if (!font)
    {
        fprintf(stderr, "Erro ao carregar fonte.\n");
        return 2;
    }

    if (!(disp = al_create_display(SCR_W, SCR_H))){
        fprintf(stderr, "Falha ao criar display.\n");
        return 2;
    }

    al_hide_mouse_cursor(disp);
    
    cursor = load_bitmap_at_size("resources/cursor.png", 30, 30);
    if (!cursor) 
    {
        fprintf(stderr, "Falha ao criar cursor.\n");
        return 3;
    }

    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_display_event_source(disp));
    al_register_event_source(queue, al_get_timer_event_source(timer));

    g = cria_gamebox(SCR_W, SCR_H);

    b = cria_struct_ball();
    b->balls[0]->x = g->w / 2 + g->x;
    b->balls[0]->y = g->h - b->raio * 2;
    b = insere_ball(b, 1);

    al_start_timer(timer);
    while(1)
    {
        al_get_mouse_state(&msestate);
        al_get_keyboard_state(&kbdstate);

        al_wait_for_event(queue, &event);

        switch (event.type)
        {
            case ALLEGRO_EVENT_DISPLAY_CLOSE:
                done = true;
                break;
        }

        if (done || al_key_down(&kbdstate, ALLEGRO_KEY_ESCAPE))
            break;

        // if (al_key_down(&kbdstate, ALLEGRO_KEY_F4))
        // {
        //     b->balls[0]->x = SCR_W / 2;
        //     b->balls[0]->y = SCR_H - b->raio - 30;
        //     b->balls[0]->sx = 0;
        //     b->balls[0]->sy = 0;

        //     al_start_timer(timer);
        // }

        al_clear_to_color(al_map_rgb(30, 30, 30));
        al_draw_filled_rectangle(g->x, g->y, g->w, g->h, al_map_rgb(20, 20, 20));
        al_draw_text(font, al_map_rgb(230,230,230), SCR_W/4, g->y * 0.3, 1, "BALLZ");
        al_draw_textf(font, al_map_rgb(230,230,230), SCR_W - SCR_W/4, g->y * 0.3, 1, "%d", points);
        if (b->launch == false)
        {
            al_draw_bitmap(b->img, b->balls[0]->x, b->balls[0]->y, 0);
            if (!mspress && al_mouse_button_down(&msestate, 1))
            {
                x0 = msestate.x;
                y0 = msestate.y;
                mspress = true;
            }

            if (mspress && ALLEGRO_EVENT_MOUSE_AXES)
            {
                delta_x = x0 - msestate.x;
                delta_y = y0 - msestate.y;
                if (delta_y < -20)
                {
                    if (delta_y < -SCR_H/3)
                        delta_y = -SCR_H / 3;
                    al_draw_line(b->balls[0]->x + b->raio, b->balls[0]->y + b->raio, b->balls[0]->x + b->raio + delta_x, b->balls[0]->y + delta_y + b->raio, al_map_rgb(255, 255, 255), 3);
                }
            }

            if (mspress && !al_mouse_button_down(&msestate, 1))
            {
                for (b->l_ctr=0; b->l_ctr < b->n; b->l_ctr += 1){
                    lancar_bola(b->balls[b->l_ctr], delta_x, delta_y, b->vel);
                    b->launch = true;
                }
                l_timer = al_get_time();
                n_timer = al_get_time() - l_timer;
                mspress = false;
            }
        }
        for (int i=0; i < b->n; i++){
            if (b->balls[i]->sx == 0 && b->balls[i]->sy == 0)
            {
                b->balls[i]->y = b->balls[0]->y;
                b->balls[i]->x = b->balls[0]->x;
                if (b->balls[i]->move == true)
                    b->l_ctr -= 1;
                b->balls[i]->move = false;
            }
            else
            {
                if (b->balls[i]->move == false && n_timer <= 0.1 * i){
                    n_timer = al_get_time() - l_timer;
                    b->balls[i]->x += 0;
                    b->balls[i]->y += 0;
                }
                else
                {
                    b->balls[i]->x += b->balls[i]->sx;
                    b->balls[i]->y += b->balls[i]->sy;
                    b->balls[i]->move = true;
                }
                
                if (b->balls[i]->x <= 0)
                {
                    b->balls[i]->x = 0 - b->balls[i]->sx/3;
                    b->balls[i]->sx = -b->balls[i]->sx;
                }
                if (b->balls[i]->x >= SCR_W - b->raio * 2)
                {
                    b->balls[i]->x = SCR_W - b->raio * 2 - b->balls[i]->sx/3;
                    b->balls[i]->sx = -b->balls[i]->sx;
                }

                if (b->balls[i]->y > g->h - b->raio * 2)
                {
                    b->balls[i]->y = g->h - b->raio * 2;
                    b->balls[i]->sx = 0;
                    b->balls[i]->sy = 0;
                }
                if (b->balls[i]->y <= g->y)
                {
                    b->balls[i]->y = g->y;
                    b->balls[i]->sy = -b->balls[i]->sy;
                }                
            }
            al_draw_bitmap(b->img, b->balls[i]->x, b->balls[i]->y, 0);
        }
        if (b->launch == true && b->l_ctr == 0)
        {
            b->launch = false;
            points++;
        }
        al_draw_bitmap(cursor, msestate.x, msestate.y, 0);
        al_flip_display();
    }

    al_destroy_display(disp);
    al_destroy_timer(timer);
    al_destroy_event_queue(queue);
    al_destroy_bitmap(cursor);
    destroi_struct_ball(b);

    return 0;
}
