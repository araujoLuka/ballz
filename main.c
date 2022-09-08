#include "libball.h"
#include "game.h"
#include "engine.h"
#include <stdlib.h>
#include <stdio.h>

#define NUM_BUTTONS 3

int main()
{
    engine_t *e;
    list_b *b;
    ball_t *ball;
    gb_t *g;
    bool done = false, mspress = false;
    float x0 = 0, y0 = 0;
    double delta_x, delta_y;
    double current_time, last_time, delta_time;
    double l_timer;
    int points = 1, t_mult = 1;

    if (!(e = start_engine()))
        return 1;

    
    g = cria_gamebox(SCR_W, SCR_H);
    b = make_ball_list(g->w, g->h);
    ball = b->ini;
    insert_ball(b, 69);

    al_start_timer(e->timer);
    last_time = al_get_time();
    while(1)
    {
        current_time = al_get_time();
        delta_time = (current_time - last_time) * t_mult;
        ball = b->ini;

        al_get_mouse_state(&e->msestate);
        al_get_keyboard_state(&e->kbdstate);

        al_wait_for_event(e->queue, &e->event);

        switch (e->event.type)
        {
            case ALLEGRO_EVENT_DISPLAY_CLOSE:
                done = true;
                break;
        }

        if (done || al_key_down(&e->kbdstate, ALLEGRO_KEY_ESCAPE))
            break;

        if (al_key_down(&e->kbdstate, ALLEGRO_KEY_F4))
        {
            if (t_mult == 1)
                t_mult = 2;
            else
                t_mult = 1;
        //     b->ini->x = SCR_W / 2;
        //     b->ini->y = SCR_H - b->raio - 30;
        //     b->ini->sx = 0;
        //     b->ini->sy = 0;

        //     al_start_timer(timer);
        }

        if (b->launch == false)
        {
            if (!mspress && al_mouse_button_down(&e->msestate, 1))
            {
                x0 = e->msestate.x;
                y0 = e->msestate.y;
                mspress = true;
            }

            if (mspress && ALLEGRO_EVENT_MOUSE_AXES)
            {
                delta_x = x0 - e->msestate.x;
                delta_y = y0 - e->msestate.y;
            }

            if (mspress && !al_mouse_button_down(&e->msestate, 1))
            {
                if (delta_y < -20)
                {
                    for (b->l_ctr=0; b->l_ctr < b->tam; b->l_ctr++)
                    {
                        set_ball_speed(ball, delta_x, delta_y, b->vel);
                        ball = ball->next;
                    }
                    ball = b->ini;
                    b->launch = true;
                    l_timer = al_get_time();
                }
                mspress = false;
            }
        }
        for (int i=0; i < b->tam && ball != NULL; i++){
            if (ball->sx == 0 && ball->sy == 0)
            {
                ball->x = b->ini->x;
                ball->y = b->ini->y;
                if (ball->move == true)
                    b->l_ctr -= 1;
                ball->move = false;
            }
            else
            {
                if (current_time - l_timer > (i * 0.05) || ball->move == true)
                {
                    ball->x += ball->sx * delta_time;
                    ball->y += ball->sy * delta_time;
                    ball->move = true;
                    
                    collide_left(ball, 0);
                    collide_right(ball, SCR_W - b->raio * 2);
                    collide_top(ball, g->y);
                    collide_bottom(ball, g->h - b->raio * 2 - 0.5);
                }
            }
            ball = ball->next;
        }
        if (b->launch == true && b->l_ctr == 0)
        {
            delta_y = 0;
            b->launch = false;
            points++;
            t_mult = 1;
        }

        draw_game(e, b, g, delta_x, delta_y, points);
        last_time = current_time;
    }

    e = end_engine(e);
    b = destroy_ball_list(b);

    return 0;
}
