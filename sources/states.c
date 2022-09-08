#include "states.h"
#include "game.h"
#include <stdio.h>

void state_menu(engine_t *e, states *st)
{
    vet_t ms, v;
    vet_t play_button;
    float aux, x;
    int button_size = 50;
    bool done = false, st_swt = false;
    
    play_button = vet_make(SCR_W/2, SCR_H * 0.7);

    al_start_timer(e->timer);

    if (e->event.keyboard.keycode == ALLEGRO_KEY_ESCAPE)
        st_swt = true;

    while (1)
    {
        al_get_mouse_state(&e->msestate);
        al_get_keyboard_state(&e->kbdstate);
        al_wait_for_event(e->queue, &e->event);

        ms = vet_make(e->msestate.x, e->msestate.y);
        v = vet_dif(play_button, ms);
        x = vet_len(v);

        switch (e->event.type)
        {
            case ALLEGRO_EVENT_DISPLAY_CLOSE:
                done = true;
                break;
            case ALLEGRO_EVENT_KEY_UP:
                st_swt = false;
                break;
        }

        if (done || (!st_swt && al_key_down(&e->kbdstate, ALLEGRO_KEY_ESCAPE)))
        {
            *st = END;
            break;
        }

        if (al_mouse_button_down(&e->msestate, 1) && x <= button_size)
        {
            *st = GAME;
            break;
        }

        al_clear_to_color(al_map_rgb(20, 20, 20));

        aux = al_get_font_line_height(e->fonts->f60)/2;
        al_draw_text(e->fonts->f60, al_map_rgb(235, 235, 250), SCR_W/2, SCR_H*0.2, -1, "Ballz");

        al_draw_filled_circle(play_button.x, play_button.y, button_size, al_map_rgb(180, 230, 40));
        if (x <= button_size)
            al_draw_circle(play_button.x, play_button.y, button_size + 1, al_map_rgb(240, 240, 240), 3);
        aux = al_get_font_line_height(e->fonts->f26)/2;
        al_draw_text(e->fonts->f26, al_map_rgb(30, 30, 30), play_button.x, play_button.y - aux, -1, "PLAY");

        al_draw_bitmap(e->cursor, e->msestate.x, e->msestate.y, 0);
        
        al_flip_display();
    }
}

void state_game(engine_t *e, states *st)
{
    game_t *gs;
    vet_t start, delta;
    g_states gst;
    bool mspress = false, play = true;
    int nb = 0;
    float lt = 0;

    if (!game_start(e, &gs, &start, &delta, &gst))
        return;

    while(play)
        switch (game_update(e, gs, &mspress, &start, &delta, &nb, &lt, &gst))
        {
        case 0:
            *st = MENU;
            play = false;
            break;
        
        case 2:
            game_destroy(&gs);
            if (!game_start(e, &gs, &start, &delta, &gst))
                return;
            break;
        }

    game_destroy(&gs);
}