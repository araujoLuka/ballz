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

int game_start(engine_t *e, game_t **gs, vet_t *start, vet_t *delta, g_states *gst)
{
    if (!(*gs = game_make(SCR_W, SCR_H)))
        return 0;

    al_start_timer(e->timer);
    (*gs)->last_t = al_get_time();
    block_insert_row((*gs)->m, (*gs)->g->x, (*gs)->g->y, (*gs)->points);
    block_move((*gs)->m);
    ball_insert((*gs)->b, 0);
    *start = vet_make(0,0);
    *delta = vet_make(0,0);
    *gst = ANIM;

    return 1;
}

int game_update(engine_t *e, game_t *gs, bool *mspress, vet_t *aim1, vet_t *aim2, int *nb, float *lt, g_states *gst)
{
    g_states last;
    gs->curr_t = al_get_time();
    gs->delta_t = (gs->curr_t - gs->last_t) * gs->t_mult;

    // game_test(e, gs->b, nb);

    if (*gst != PAUSE)
        last = *gst;
    
    *gst = game_events(e, *gst, last, &gs->t_mult);
    
    switch (*gst)
    {
    case ANIM:
        *gst = AIM;
        break;
    case AIM:
        *gst = game_aim(e, gs, mspress, aim1, aim2);
        break;
    case LAUNCH: 
        *gst = game_launch(e, gs, aim2, lt, nb);
        break;
    case RESET: 
        *gst = game_reset(gs);
        break;
    case OVER:
        return 0; // game_over(gs);
        break;
    case PAUSE:
        // game_paused();
        break;
    case RESTART:
        return 2;
        break;
    }

    game_draw(e, gs, aim2, *gst);
    gs->last_t = gs->curr_t;

    return 1;
}

g_states game_events(engine_t *e, g_states gst, g_states lst, float *scale)
{
    al_get_mouse_state(&e->msestate);
    al_get_keyboard_state(&e->kbdstate);

    al_wait_for_event(e->queue, &e->event);

   switch (e->event.type)
    {
    case ALLEGRO_EVENT_DISPLAY_CLOSE:
        return OVER;

    case ALLEGRO_EVENT_KEY_DOWN:
        if (e->event.keyboard.keycode == ALLEGRO_KEY_F9)
        {
            if (gst == PAUSE)
                return lst;
            return PAUSE;
        }
    }

    if (al_key_down(&e->kbdstate, ALLEGRO_KEY_ESCAPE))
        return OVER;

    if (al_key_down(&e->kbdstate, ALLEGRO_KEY_F5))
        return RESTART;
        
    if (al_key_down(&e->kbdstate, ALLEGRO_KEY_F4))
    {
        if (gst == LAUNCH)
            *scale += 0.5f;
    }   

    return gst;
}

g_states game_aim(engine_t *e, game_t *gs, bool *mspress, vet_t *start, vet_t *delta)
{
    ball_t *b = gs->b;

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

    if (b->launch)
        return LAUNCH;

    return AIM;
}

g_states game_launch(engine_t *e, game_t *gs, vet_t *v, float *lt, int *nb)
{
    ball_t *b;
    nodo_b *ball;
    
    b = gs->b;
    ball = gs->b->ini;

    *lt = al_get_time() - gs->b->l_tmr;

    for (int i=0; i < b->tam && ball != NULL; i++)
    {
        if (ball->sx == 0 && ball->sy == 0)
        {
            if (ball->move == true)
            {
                if (b->l_ctr == b->tam)
                    b->first = ball;
                b->l_ctr -= 1;
            }
            ball->cx = b->first->cx;
            ball->cy = b->first->cy;
            ball->move = false;
        }
        else
        {
            if (*lt >= 0.1 * i || ball->move == true)
            {
                if (ball->move == false)
                    b->n--;
                ball->cx += (ball->sx) * (gs->delta_t);
                ball->cy += (ball->sy) * (gs->delta_t);
                ball->move = true;
                
                ball_collide_left(ball, b->raio, 0);
                ball_collide_right(ball, b->raio, SCR_W);
                ball_collide_top(ball, b->raio, gs->g->y);
                ball_collide_bottom(ball, b->raio, gs->g->h);
                ball_collide_block(ball, gs->m, b->raio, nb, &gs->coins);
            }
        }
        ball = ball->next;
    }
    if (game_launch_end(e, gs, v, nb))
        return RESET;
    
    return LAUNCH;
}

int game_launch_end(engine_t *e, game_t *gs, vet_t *v, int *nb)
{
    if (gs->b->l_ctr == 0)
    {
        v->x = 0;
        v->y = 0;
        gs->delta_t = 0;
        gs->b->launch = false;
        gs->points += 1;
        gs->t_mult = 1;
        ball_insert(gs->b, *nb);
        block_insert_row(gs->m, gs->g->x, gs->g->y, gs->points);
        *nb = 0;

        return 1;
    }

    return 0;
}

g_states game_reset(game_t *gs)
{
    if (gs->m->rows[ROWS_B-1].n_blocks > 0)
    {
        al_rest(1);
        printf("Game Over\n");
        return OVER;
    }
    block_move(gs->m);

    return ANIM;
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

void game_draw(engine_t *e, game_t *gs, vet_t *vector, g_states gst)
{
    ball_t *b = gs->b;
    box_t *g = gs->g;
    matrix_bl *m = gs->m;
    vet_t b_ctr_pos;

    al_clear_to_color(al_map_rgb(30, 30, 30));
    al_draw_filled_rectangle(g->x, g->y, g->w, g->h, al_map_rgb(20, 20, 20));
    al_draw_filled_circle(3*SCR_W/4 - b->raio*4, g->y * 0.5, b->raio*1.5, al_map_rgb(240,240,70));
    al_draw_filled_circle(3*SCR_W/4 - b->raio*4, g->y * 0.5, b->raio*1, al_map_rgb(30,30,30));
    al_draw_textf(e->fonts->f26, al_map_rgb(230,230,230), 3*SCR_W/4 + 5, g->y * 0.3, 1, "%d", gs->coins);
    al_draw_textf(e->fonts->f26, al_map_rgb(230,230,230), SCR_W/2, g->y * 0.3, 1, "%d", gs->points);
    
    block_draw(e, m);
    ball_draw(b);

    if (b->launch)
        b_ctr_pos = vet_make(vector->x, vector->y);
    else
    {
        b_ctr_pos = vet_make(b->ini->cx, b->ini->cy);
        game_aim_draw(b, vector->x, vector->y);
    }
    if (b->n > 0)
        al_draw_textf(e->fonts->f18, al_map_rgb(230, 230, 230), b_ctr_pos.x, b_ctr_pos.y + 10, -1, "%d x", b->n);

    if (gst == PAUSE)
        al_draw_filled_rectangle(0, 0, SCR_W, SCR_H, al_map_rgba(0, 0, 0, 100));

    al_draw_bitmap(e->cursor, e->msestate.x, e->msestate.y, 0);
    al_flip_display();
}

void game_aim_draw(ball_t *b, float dx, float dy)
{
    float aim_x, aim_y;
    aim_x = b->ini->cx;
    aim_y = b->ini->cy;

    if (dy < -20)
    {
        al_draw_line(aim_x, aim_y, aim_x + dx, aim_y + dy, al_map_rgb(255, 255, 255), 2);
    }
}

void game_destroy(game_t **gs)
{
    box_destroy((*gs)->g);
    block_matrix_destroy(&(*gs)->m);
    ball_list_destroy(&(*gs)->b);

    free((*gs));
    (*gs) = NULL;
}