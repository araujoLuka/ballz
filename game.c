#include "game.h"
#include <stdio.h>
#include <stdlib.h>

game_t *game_make(float scr_w, float scr_h)
{
    game_t *gs;

    gs = malloc(sizeof(game_t));
    if (!gs)
    {
        fprintf(stderr, "Falha ao alocar estrutura central do jogo\n");
        return NULL;
    }

    gs->g = box_make(0, SCR_H * 0.1, SCR_W, SCR_H * 0.9);
    gs->m = block_matrix_make(gs->g->w);
    gs->b = ball_list_make(gs->g->w, gs->g->h);
    if (!gs->g || !gs->m || !gs->b)
    {
        fprintf(stderr, "Falha ao alocar estruturas principais do jogo\n");
        game_destroy(&gs);
        return NULL;
    }
    gs->points = 1;
    gs->curr_t = 0;
    gs->last_t = 0;
    gs->delta_t = 0;
    gs->t_mult = 1;
    gs->coins = 0;

    return gs;
}

void game_destroy(game_t **gs)
{
    box_destroy((*gs)->g);
    block_matrix_destroy(&(*gs)->m);
    ball_list_destroy(&(*gs)->b);

    free((*gs));
    (*gs) = NULL;
}

int game_events(engine_t *e)
{
    al_get_mouse_state(&e->msestate);
    al_get_keyboard_state(&e->kbdstate);

    al_wait_for_event(e->queue, &e->event);
   
    switch (e->event.type)
    {
        case ALLEGRO_EVENT_DISPLAY_CLOSE:
            return 0;
            break;
    }

    if (al_key_down(&e->kbdstate, ALLEGRO_KEY_ESCAPE))
        return 0;

    if (al_key_down(&e->kbdstate, ALLEGRO_KEY_F5))
        return 2;

    return 1;
}

int game_aim(game_t *gs, engine_t *e, ball_t *b, bool *mspress, vet_t *start, vet_t *delta)
{

    if (!*mspress && al_mouse_button_down(&e->msestate, 1))
    {
        start->x = e->msestate.x;
        start->y = e->msestate.y;
        *mspress = true;
    }

    if (*mspress && ALLEGRO_EVENT_MOUSE_AXES)
    {
        delta->x = start->x - e->msestate.x;
        delta->y = start->y - e->msestate.y;
    }

    if (*mspress && !al_mouse_button_down(&e->msestate, 1))
    {
        if (delta->y < -20)
        {
            ball_launch(b, delta, b->vel);
            gs->b->l_tmr = al_get_time();
        }
        delta->x = b->ini->cx;
        delta->y = b->ini->cy;
        *mspress = false;
    }

    return 1;
}

int game_start(engine_t *e, game_t **gs)
{
    if (!(*gs = game_make(SCR_W, SCR_H)))
        return 0;

    al_start_timer(e->timer);
    (*gs)->last_t = al_get_time();
    block_insert_row((*gs)->m, (*gs)->g->x, (*gs)->g->y, (*gs)->points);
    block_move((*gs)->m);
    ball_insert((*gs)->b, 0);

    return 1;
}

void game_test(engine_t *e, game_t *gs, int *nb)
{
    nodo_b *b;
    b = gs->b->ini;

    b->sx = 0;
    b->sy = 0;
    if (al_key_down(&e->kbdstate, ALLEGRO_KEY_W))
        b->sy = -4;
    if (al_key_down(&e->kbdstate, ALLEGRO_KEY_S))
        b->sy = 4;
    if (al_key_down(&e->kbdstate, ALLEGRO_KEY_A))
        b->sx = -4;
    if (al_key_down(&e->kbdstate, ALLEGRO_KEY_D))
        b->sx = 4;
    
    ball_collide_left(b, gs->b->raio, 0);
    ball_collide_right(b, gs->b->raio, gs->g->w);
    ball_collide_top(b, gs->b->raio, gs->g->y);
    ball_collide_bottom(b, gs->b->raio, gs->g->h - gs->b->raio * 2 - 0.5);
    ball_collide_block(b, gs->m, gs->b->raio, nb, &gs->coins);
    b->cx += b->sx;
    b->cy += b->sy;
}

int game_update(engine_t *e, game_t *gs, bool *mspress, vet_t *aim1, vet_t *aim2, int *nb, float *lt)
{
    nodo_b *b;

    gs->curr_t = al_get_time();
    gs->delta_t = (gs->curr_t - gs->last_t) * gs->t_mult;
    b = gs->b->ini;

    // game_test(e, gs->b, nb);

    switch (game_events(e))
    {
    case 0:
        return 0;
        break;
    
    case 2:
        return 2;
        break;
    } 

    if (gs->b->launch == false)
        game_aim(gs, e, gs->b, mspress, aim1, aim2);
    else
    {
        *lt = al_get_time() - gs->b->l_tmr;
        for (int i=0; i < gs->b->tam && b != NULL; i++)
        {
            if (b->sx == 0 && b->sy == 0)
            {
                b->cx = gs->b->ini->cx;
                b->cy = gs->b->ini->cy;
                if (b->move == true)
                    gs->b->l_ctr -= 1;
                b->move = false;
            }
            else
            {
                if (*lt >= 0.1 * i || b->move == true)
                {
                    if (b->move == false)
                        gs->b->n--;
                    b->cx += (b->sx) * (gs->delta_t);
                    b->cy += (b->sy) * (gs->delta_t);
                    b->move = true;
                    
                    ball_collide_left(b, gs->b->raio, 0);
                    ball_collide_right(b, gs->b->raio, SCR_W);
                    ball_collide_top(b, gs->b->raio, gs->g->y);
                    ball_collide_bottom(b, gs->b->raio, gs->g->h);
                    ball_collide_block(b, gs->m, gs->b->raio, nb, &gs->coins);
                }
            }
            b = b->next;
        }

        if (gs->b->l_ctr == 0)
        {
            aim2->x = 0;
            aim2->y = 0;
            gs->delta_t = 0;
            gs->b->launch = false;
            gs->points += 1;
            gs->t_mult = 1;
            ball_insert(gs->b, *nb);
            *nb = 0;
            block_insert_row(gs->m, gs->g->x, gs->g->y, gs->points);
            if (gs->m->rw_c->n_blocks[ROWS_B-1] > 0)
            {
                draw_game(e, gs->b, gs->g, gs->m, aim2->x, aim2->y, gs->points, gs->coins);
                al_rest(1);
                printf("Game Over\n");
                return 0;
            }
            block_move(gs->m);
            nb = 0;
        }
    }

    draw_game(e, gs->b, gs->g, gs->m, aim2->x, aim2->y, gs->points, gs->coins);
    gs->last_t = gs->curr_t;

    return 1;
}

void game_state(engine_t *e)
{
    game_t *gs;
    vet_t start, delta;
    bool mspress = false;
    int nb = 0;
    float lt = 0;

    if (!game_start(e, &gs))
        return;

    start = vet_make(0,0);
    delta = vet_make(0,0);

    while(1)
        switch (game_update(e, gs, &mspress, &start, &delta, &nb, &lt))
        {
        case 0:
            return;
            break;
        
        case 2:
            game_destroy(&gs);
            if (!game_start(e, &gs))
                return;

            start = vet_make(0,0);
            delta = vet_make(0,0);
            break;
        }

    game_destroy(&gs);
}