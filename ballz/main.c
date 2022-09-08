#include "ball.h"
#include "game.h"
#include "engine.h"
#include "block.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#define NUM_BUTTONS 3

int main()
{
    engine_t *e;
    gb_t *g;
    matrix_bl *m;
    list_b *b;
    ball_t *ball;
    int erro = 0;
    bool done = false, mspress = false;
    float x0 = 0, y0 = 0;
    double delta_x = 0, delta_y = 0;
    double current_time, last_time, delta_time;
    double l_timer = 0;
    int points = 1, t_mult = 1, nb = 0, coins = 0;

    srand(time(NULL));

    e = start_engine();
    g = game_make_box_p(SCR_W, SCR_H);
    m = block_matrix_make(g->w);
    b = ball_list_make(g->w, g->h);
    
    if (!e || !g || !m || !b)
        erro = 1;
    else
    {
        al_start_timer(e->timer);
        last_time = al_get_time();
        block_insert_row(m, g->x, g->y, points);
        block_move(m);
    }

    while(!erro)
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
                t_mult = 3;
            else
                t_mult = 1;
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
                    ball_launch(b, delta_x, delta_y, b->vel);
                    l_timer = al_get_time();
                }
                mspress = false;
            }
        }
        for (int i=0; i < b->tam && ball != NULL; i++)
        {
            if (ball->sx == 0 && ball->sy == 0)
            {
                ball->cx = b->ini->cx;
                ball->cy = b->ini->cy;
                if (ball->move == true)
                    b->l_ctr -= 1;
                ball->move = false;
            }
            else
            {
                if (l_timer + i * 0.1 <= current_time || ball->move == true)
                {
                    ball->cx += (int)(ball->sx) / 1000 * (delta_time);
                    ball->cy += (int)(ball->sy) / 1000 * (delta_time);
                    ball->move = true;
                    
                    ball_collide_left(ball, b->raio, 0);
                    ball_collide_right(ball, b->raio, SCR_W - b->raio * 2);
                    ball_collide_top(ball, b->raio, g->y);
                    if (ball_collide_bottom(ball, b->raio, g->h - b->raio * 2 - 0.5))
                    {
                        ball->sx = 0;
                        ball->sy = 0;
                    }
                    ball_collide_block(ball, m, b->raio, &nb, &coins);
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
            ball_insert(b, nb);
            block_insert_row(m, g->x, g->y, points);
            if (!block_move(m))
            {
                printf("Game Over\n");
                break;
            }
            nb = 0;
        }

        draw_game(e, b, g, m, delta_x, delta_y, points, coins);
        last_time = current_time;
    }

    end_engine(&e);
    game_destroy(&g);
    // block_destroy(&m);
    ball_list_destroy(&b);

    return 0;
}
