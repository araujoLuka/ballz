#include "states.h"
#include "game.h"
#include <stdio.h>

void state_start(engine_t *e, u_data **d, states *st)
{
    ALLEGRO_BITMAP *load;
    vec_t ms;
    box_t yes_b, no_b;
    int load_size, t;
    bool done = false;
    float rt;

    t = 0;
    load_size = 60;
    load = al_load_bitmap("resources/models/loading2.png");
    yes_b = box_make(SCR_W*0.25, SCR_H*0.6, SCR_W*0.2, SCR_H*0.07);
    no_b = box_make(SCR_W*0.55, SCR_H*0.6, SCR_W*0.2, SCR_H*0.07);

    rt = 1;
    while (rt < 5)
    {
        al_clear_to_color(al_map_rgb(20, 20, 20));
        al_draw_rotated_bitmap(load, load_size/2, load_size/2, SCR_W * 0.85, SCR_H * 0.85, rt, 0);
        al_flip_display();
        rt += 0.05;
    }

    *d = data_fetch();
    *st = MENU;

    if (*d == NULL)
    {
        al_start_timer(e->timer);

        while (t != 2)
        {
            al_get_mouse_state(&e->msestate);
            al_get_keyboard_state(&e->kbdstate);
            al_wait_for_event(e->queue, &e->event);

            ms = vector_make(e->msestate.x, e->msestate.y);

            switch (e->event.type)
            {
                case ALLEGRO_EVENT_DISPLAY_CLOSE:
                    done = true;
                    break;
            }

            if (done || al_key_down(&e->kbdstate, ALLEGRO_KEY_ESCAPE))
            {
                *st = END;
                break;
            }

            if (al_mouse_button_down(&e->msestate, 1))
            {
                if (box_inside(ms, yes_b))
                {
                    if (t == 0)
                        *d = data_create();
                    t = 2;
                    al_rest(0.05);
                }
                if (box_inside(ms, no_b))
                {
                    if (t == 0)
                        t = 1;
                    else 
                        t = 0;
                    al_rest(0.05);
                }
            }

            al_clear_to_color(al_map_rgb(20, 20, 20));

            if (t == 0)
            {
                al_draw_filled_rounded_rectangle(SCR_W*0.15+5,SCR_H*0.4+10,SCR_W*0.85+5,SCR_H*0.5+10,5,5, al_map_rgb(130, 90, 0));
                al_draw_filled_rounded_rectangle(SCR_W*0.15,SCR_H*0.4,SCR_W*0.85,SCR_H*0.5,5,5, al_map_rgb(222, 175, 0));
                draw_text(e->fonts->r26, al_map_rgb(255, 255, 255), SCR_W*0.5, SCR_H*0.43, -1, "Savefile not found!");
                draw_text(e->fonts->r26, al_map_rgb(255, 255, 255), SCR_W*0.5, SCR_H*0.47, -1, "Want to create one?");
            }
            else if (t == 1)
            {
                al_draw_filled_rounded_rectangle(SCR_W*0.1+5,SCR_H*0.4+10,SCR_W*0.9+5,SCR_H*0.5+10,5,5, al_map_rgb(130, 30, 0));
                al_draw_filled_rounded_rectangle(SCR_W*0.1,SCR_H*0.4,SCR_W*0.9,SCR_H*0.5,5,5, al_map_rgb(222, 120, 0));
                draw_text(e->fonts->r26, al_map_rgb(255, 255, 255), SCR_W*0.5, SCR_H*0.43, -1, "Game progress will not be saved");
                draw_text(e->fonts->r26, al_map_rgb(255, 255, 255), SCR_W*0.5, SCR_H*0.47, -1, "Want to continue?");
            }

            draw_button(e->fonts, al_map_rgb(80, 180, 100), yes_b, "YES", ms);
            draw_button(e->fonts, al_map_rgb(180, 50, 100), no_b, "NO", ms);

            al_draw_bitmap(e->cursor, e->msestate.x, e->msestate.y, 0);
            
            al_flip_display();
        }
    }
}

void state_menu(engine_t *e, u_data *d, states *st)
{
    ALLEGRO_BITMAP *coin;
    vec_t ms;
    box_t play_button;
    circ_t help_btn;
    bool done = false;
    int FPS;

    coin = al_load_bitmap("resources/models/coin1.png");
    e->last_t = al_get_time();
    play_button = box_make(SCR_W*0.35, SCR_H * 0.65, SCR_W*0.3, SCR_H*0.1);
    help_btn = circle_make(SCR_W*0.5, SCR_H * 0.85, SCR_H*0.04);

    al_start_timer(e->timer);

    while (1)
    {
        al_get_mouse_state(&e->msestate);
        al_get_keyboard_state(&e->kbdstate);
        al_wait_for_event(e->queue, &e->event);

        ms = vector_make(e->msestate.x, e->msestate.y);

        switch (e->event.type)
        {
            case ALLEGRO_EVENT_DISPLAY_CLOSE:
                done = true;
                break;
        }

        if (done || al_key_down(&e->kbdstate, ALLEGRO_KEY_ESCAPE))
        {
            *st = END;
            break;
        }

        if (al_mouse_button_down(&e->msestate, 1))
        {
            if (box_inside(ms, play_button))
            {
                *st = GAME;
                break;
            }
            else if (circle_inside(ms, help_btn))
            {
                *st = HELP;
                break;
            }

        } 

        al_clear_to_color(al_map_rgb(20, 20, 20));

        if (d != NULL)
        {
            al_draw_bitmap(coin, SCR_W*0.96-25/2, SCR_H*0.05-25/2, 0);
            draw_num(e->fonts->r26, al_map_rgb(255, 255, 255), SCR_W*0.89, SCR_H*0.05, 1, d->coins);
        }

        draw_text(e->fonts->l60, al_map_rgb(230, 230, 255), SCR_W/2, SCR_H*0.25, -1, "Ballz");
        draw_text(e->fonts->r30, al_map_rgb(230, 230, 255), SCR_W/2, SCR_H*0.3, -1, "Deluxe Edition");
        draw_text(e->fonts->l10, al_map_rgb(230, 230, 255), SCR_W/2, SCR_H*0.34, -1, "Beta");

        if (d != NULL && d->sb_scores[0] > 0)
        {
            draw_text(e->fonts->r18, al_map_rgb(255, 255, 255), SCR_W/2, SCR_H*0.5, -1, "HIGH SCORE");
            draw_num(e->fonts->r40, al_map_rgb(255, 255, 255), SCR_W/2, SCR_H*0.55, -1, d->sb_scores[0]);
        }

        draw_button(e->fonts, al_map_rgb(50, 100, 150), play_button, "PLAY", ms);
        draw_icon("resources/models/question.png", al_map_rgb(222, 60, 80), help_btn, ms);

        al_draw_bitmap(e->cursor, e->msestate.x, e->msestate.y, 0);
        e->curr_t = al_get_time();
        e->delta_t = (e->curr_t - e->last_t);
        e->last_t = e->curr_t;
        
        FPS = 1 / (e->delta_t);
        al_draw_textf(e->fonts->l10, al_map_rgb(50,255,50), 5, 5, 0, "FPS: %d", (int)(FPS));
        al_flip_display();
    }
}

void state_help(engine_t *e, states *st)
{
    ALLEGRO_BITMAP *help;
    vec_t ms;
    circ_t back_button;
    bool done = false;
    int FPS;

    help = al_load_bitmap("resources/models/help1.png");
    back_button = circle_make(SCR_H*0.04, SCR_H*0.06, SCR_H*0.03);

    al_start_timer(e->timer);

    while (1)
    {
        al_get_mouse_state(&e->msestate);
        al_get_keyboard_state(&e->kbdstate);
        al_wait_for_event(e->queue, &e->event);

        ms = vector_make(e->msestate.x, e->msestate.y);

        switch (e->event.type)
        {
            case ALLEGRO_EVENT_DISPLAY_CLOSE:
                done = true;
                break;
        }

        if (done || al_key_down(&e->kbdstate, ALLEGRO_KEY_ESCAPE))
        {
            al_rest(0.2);
            if (st != NULL)
                *st = MENU;
            break;
        }

        if (al_mouse_button_down(&e->msestate, 1))
        {
            if (circle_inside(ms, back_button))
            {
                al_rest(0.1);
                if (st != NULL)
                    *st = MENU;
                break;
            }
        } 

        al_clear_to_color(al_map_rgb(20, 20, 20));

        draw_icon("resources/models/back.png", al_map_rgb(60, 60, 60), back_button, ms);

        draw_text(e->fonts->r30, al_map_rgb(255, 255, 255), SCR_W/2, SCR_H*0.05, -1, "HOW TO PLAY");

        float img_width = al_get_bitmap_width(help);
        float img_height = al_get_bitmap_height(help);
        al_draw_bitmap(help, SCR_W*0.5 - img_width/2, SCR_H*0.5 - img_height/2, 0);

        draw_text(e->fonts->r18, al_map_rgb(255, 255, 255), SCR_W/2, SCR_H*0.97, 1, "Created by Lucas Araujo");

        al_draw_bitmap(e->cursor, e->msestate.x, e->msestate.y, 0);
        e->curr_t = al_get_time();
        e->delta_t = (e->curr_t - e->last_t);
        e->last_t = e->curr_t;
        
        FPS = 1 / (e->delta_t);
        al_draw_textf(e->fonts->l10, al_map_rgb(50,255,50), 5, 5, 0, "FPS: %d", (int)(FPS));
        al_flip_display();
    }
}

void state_game(engine_t *e, u_data *d, states *st)
{
    game_t *gs;
    vec_t start, delta;
    bool mspress = false, play = true;
    int nb = 0;
    float lt = 0;

    if (!game_start(e, &gs, &d, &start, &delta))
        return;

    while(play)
    {
        switch (game_update(e, gs, &mspress, &start, &delta, &nb, &lt))
        {
        case 0:
            *st = MENU;
            play = false;
            break;
        
        case 2:
            *st = GAME;
            play = false;
            break;
        }
    }

    data_record(*gs->d);
    game_destroy(&gs);
}