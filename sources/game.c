#include "game.h"
#include <stdio.h>
#include <stdlib.h>

#define MOVE_BLOCKS 1

game_t *game_make(float scr_w, float scr_h)
{
    game_t *gs;

    gs = malloc(sizeof(game_t));
    if (!gs)
    {
        fprintf(stderr, "Falha ao alocar estrutura central do jogo\n");
        return NULL;
    }

    gs->gm_box = box_make(0, SCR_H * 0.1, SCR_W, SCR_H * 0.8);
    gs->m = block_matrix_make(gs->gm_box.x2);
    gs->b = ball_list_make(gs->gm_box.x2, gs->gm_box.y2);
    if (!gs->m || !gs->b)
    {
        fprintf(stderr, "Falha ao alocar estruturas principais do jogo\n");
        game_destroy(&gs);
        return NULL;
    }

    gs->points = 1;
    gs->curr_t = 0;
    gs->last_t = 0;
    gs->delta_t = 0;
    gs->scale = 1;
    gs->coins = 0;
    gs->gst = ANIM;

    return gs;
}

int game_start(engine_t *e, game_t **gs, vec_t *start, vec_t *delta)
{
    if (!(*gs = game_make(SCR_W, SCR_H)))
        return 0;

    al_start_timer(e->timer);
    (*gs)->last_t = al_get_time();
    block_insert_row((*gs)->m, (*gs)->gm_box.x1, (*gs)->gm_box.y1, (*gs)->points);
    block_move((*gs)->m);
    ball_insert((*gs)->b, 0);
    *start = vector_make(0,0);
    *delta = vector_make(0,0);

    return 1;
}

int game_update(engine_t *e, game_t *gs, bool *mspress, vec_t *aim1, vec_t *aim2, int *nb, float *lt)
{
    g_states last;
    gs->curr_t = al_get_time();
    gs->delta_t = (gs->curr_t - gs->last_t) * gs->scale;

    // game_test(e, gs->b, nb);

    if (gs->gst != PAUSE)
        last = gs->gst;
    
    gs->gst = game_events(e, gs->gst, last, &gs->scale);
    
    switch (gs->gst)
    {
        case ANIM: game_anim(e, gs); break;

        case AIM: game_aim(e, gs, mspress, aim1, aim2); break;

        case LAUNCH: game_launch(e, gs, aim2, lt, nb); break;

        case RESET: game_reset(e, gs, aim2, nb); break;
        
        case OVER: game_over(e, gs); break;
        
        case PAUSE: game_paused(e, gs, last); break;
        
        case RESTART: return 2;

        case STOP: return 0;
    }

    al_flip_display();
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
        return STOP;
    }

    if (al_key_down(&e->kbdstate, ALLEGRO_KEY_ESCAPE))
    {
        al_rest(0.2);
        return STOP;
    }

    if (al_key_down(&e->kbdstate, ALLEGRO_KEY_N))
    {
        al_rest(0.2);
        return RESET;
    }

    if (al_key_down(&e->kbdstate, ALLEGRO_KEY_F9))
    {
        al_rest(0.2);
        if (gst == PAUSE)
            return lst;
        return PAUSE;
    }

    if (al_key_down(&e->kbdstate, ALLEGRO_KEY_F5))
        return RESTART;

    if (al_key_down(&e->kbdstate, ALLEGRO_KEY_F4))
    {
        if (gst == LAUNCH)
            *scale = 2;
    }

    return gst;
}

void game_anim(engine_t *e, game_t *gs)
{
    box_t gb = gs->gm_box;

    if (gs->gst == ANIM)
    {
        for (int i=gs->m->size; i >= 0; i-=2)
        {
            al_get_mouse_state(&e->msestate);
            game_draw(e, gs, NULL, i);
            al_flip_display();
        }
        gs->gst = AIM;
    }
    else
    {
        for (int i=1; i <= gs->m->size; i+=2)
        {
            al_get_mouse_state(&e->msestate);
            game_draw(e, gs, NULL, -i);
            al_draw_filled_rectangle(0, gb.y2, SCR_W, SCR_H, al_map_rgb(30, 30, 30));
            al_flip_display();
        }
    }
}

void game_aim(engine_t *e, game_t *gs, bool *mspress, vec_t *start, vec_t *delta)
{
    ball_t *b = gs->b;

    if (!*mspress && al_mouse_button_down(&e->msestate, 1))
    {
        start->x = e->msestate.x * 1.5;
        start->y = e->msestate.y * 1.5;
        *mspress = true;
    }

    if (*mspress && ALLEGRO_EVENT_MOUSE_AXES)
    {
        delta->x = start->x - e->msestate.x * 1.5;
        delta->y = start->y - e->msestate.y * 1.5;
    }

    if (*mspress && !al_mouse_button_down(&e->msestate, 1))
    {
        if (delta->y < -15)
        {
            ball_launch(b, delta, b->vel);
            gs->b->l_tmr = al_get_time();
        }
        delta->x = b->ini->cx;
        delta->y = b->ini->cy;
        *mspress = false;
    }

    if (b->launch)
        gs->gst = LAUNCH;

    game_draw(e, gs, delta, 0);
}

void game_launch(engine_t *e, game_t *gs, vec_t *v, float *lt, int *nb)
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
            if (ball->move)
            {
                ball->cy = b->first->cy;
                
                if (ball->cx < b->first->cx - 0.5)
                    ball->cx += (b->first->cx - ball->cx) * 0.15f;
                else if (ball->cx > b->first->cx + 0.5)
                    ball->cx -= (ball->cx - b->first->cx) * 0.15f;
                else
                {
                    ball->cx = b->first->cx;
                    b->l_ctr -= 1;
                    ball->move = false;
                }
            }
        }
        else
        {
            if (*lt >= 0.15 * i || ball->move == true)
            {
                if (ball->move == false)
                    b->n--;
                ball->cx += (ball->sx) * (gs->scale);
                ball->cy += (ball->sy) * (gs->scale);
                ball->move = true;
                
                ball_collide_left(ball, b->raio, gs->gm_box.x1);
                ball_collide_right(ball, b->raio, gs->gm_box.x2);
                ball_collide_top(ball, b->raio, gs->gm_box.y1);
                if (ball_collide_bottom(ball, b->raio, gs->gm_box.y2) && b->l_ctr == b->tam)
                {
                    b->first = ball;
                    ball->move = false;
                    b->l_ctr -= 1;
                }
                ball_collide_block(ball, gs->m, b->raio, nb, &gs->coins);
            }
        }
        ball = ball->next;
    }
    if (game_launch_end(e, gs, v, nb))
        gs->gst = RESET;
    
    game_draw(e, gs, v, 0);
}

void game_reset(engine_t *e, game_t *gs, vec_t *v, int *nb)
{
    v->x = 0;
    v->y = 0;
    gs->delta_t = 0;
    gs->b->launch = false;
    gs->scale = 1;
    ball_insert(gs->b, *nb);
    *nb = 0;
    gs->gst = ANIM;
    gs->points += 1;

    if (gs->m->rows[ROWS_B-1].n_blocks > 0)
    {
        gs->gst = OVER;
        game_anim(e, gs);
        game_draw(e, gs, NULL, -gs->m->size);
        al_draw_filled_rectangle(0, gs->gm_box.y2, SCR_W, SCR_H, al_map_rgb(20, 20, 20));
        al_draw_filled_rectangle(0, 0, SCR_W, SCR_H, al_map_rgba(100, 0, 0, 2));
        al_flip_display();
        al_rest(1);
        return;
    }

    if (MOVE_BLOCKS)
    {
        block_insert_row(gs->m, gs->gm_box.x1, gs->gm_box.y1, gs->points);
        block_move(gs->m);
    }
}

void game_over(engine_t *e, game_t *gs)
{
    vec_t ms_pos;
    box_t game_b, restart_b, menu_b;

    ms_pos = vector_make(e->msestate.x, e->msestate.y);

    game_b = gs->gm_box;
    restart_b = box_make(game_b.x2*0.3, game_b.y2*0.6, game_b.x2*0.4, game_b.y2*0.12);
    menu_b = box_make(game_b.x2*0.3, game_b.y2*0.8, game_b.x2*0.4, game_b.y2*0.12);

    if (al_mouse_button_down(&e->msestate, 1))
    {
        if (box_inside(ms_pos, restart_b))
            gs->gst = RESTART;
        else if (box_inside(ms_pos, menu_b))
            gs->gst = STOP;
    }
    else
    {
        game_draw_over(e, gs, ms_pos, restart_b, menu_b);

        gs->gst = OVER;
    }
}

void game_paused(engine_t *e, game_t *gs, g_states lst)
{
    vec_t ms_pos;
    box_t game_b, resume_b, restart_b, menu_b;

    ms_pos = vector_make(e->msestate.x, e->msestate.y);

    game_b = gs->gm_box;
    resume_b = box_make(game_b.x2*0.3, game_b.y2*0.4, game_b.x2*0.4, game_b.y2*0.1);
    restart_b = box_make(game_b.x2*0.3, game_b.y2*0.55, game_b.x2*0.4, game_b.y2*0.1);
    menu_b = box_make(game_b.x2*0.3, game_b.y2*0.7, game_b.x2*0.4, game_b.y2*0.1);
    
    if (al_mouse_button_down(&e->msestate, 1))
    {
        if (box_inside(ms_pos, resume_b))
            gs->gst = lst;
        else if (box_inside(ms_pos, restart_b))
            gs->gst = RESTART;
        else if (box_inside(ms_pos, menu_b))
            gs->gst = OVER;
        al_rest(0.02);
    }
    else
    {
        game_draw(e, gs, NULL, 0);
        game_draw_pause(e, ms_pos, game_b, resume_b, restart_b, menu_b);

        gs->gst = PAUSE;
    }
}

int game_launch_end(engine_t *e, game_t *gs, vec_t *v, int *nb)
{
    if (gs->b->l_ctr == 0)
        return 1;

    return 0;
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
    
    ball_collide_left(b, gs->b->raio, gs->gm_box.x1);
    ball_collide_right(b, gs->b->raio, gs->gm_box.x2);
    ball_collide_top(b, gs->b->raio, gs->gm_box.y1);
    ball_collide_bottom(b, gs->b->raio, gs->gm_box.y2 - gs->b->raio * 2 - 0.5);
    ball_collide_block(b, gs->m, gs->b->raio, nb, &gs->coins);
    b->cx += b->sx;
    b->cy += b->sy;
}

void game_draw(engine_t *e, game_t *gs, vec_t *vector, int anim)
{
    ball_t *b = gs->b;
    box_t g = gs->gm_box;
    matrix_bl *m = gs->m;
    vec_t b_ctr_pos;

    al_clear_to_color(al_map_rgb(30, 30, 30));
    al_draw_filled_rectangle(g.x1, g.y1, g.x2, g.y2, al_map_rgb(20, 20, 20));
    al_draw_filled_circle(3*SCR_W/4 - b->raio*4, g.y1 * 0.5, b->raio*1.4, al_map_rgb(240,240,70));
    al_draw_filled_circle(3*SCR_W/4 - b->raio*4, g.y1 * 0.5, b->raio, al_map_rgb(30,30,30));
    al_draw_textf(e->fonts->f26, al_map_rgb(230,230,230), 3*SCR_W/4 + 5, g.y1 * 0.3, 1, "%d", gs->coins);
    al_draw_textf(e->fonts->f26, al_map_rgb(230,230,230), SCR_W/2, g.y1 * 0.3, 1, "%d", gs->points);
    
    ball_draw(b);
    block_draw(e, m, anim);

    switch (gs->gst)
    {
    case AIM:
        b_ctr_pos = vector_make(b->ini->cx, b->ini->cy);
        game_draw_aim(b, vector->x, vector->y);
        if (b->n > 0)
            al_draw_textf(e->fonts->f18, al_map_rgb(255, 255, 255), b_ctr_pos.x, b_ctr_pos.y + 10, -1, "%d x", b->n);
        break;

    case LAUNCH:
        b_ctr_pos = vector_make(vector->x, vector->y);
        if (b->n > 0)
            al_draw_textf(e->fonts->f18, al_map_rgb(255, 255, 255), b_ctr_pos.x, b_ctr_pos.y + 10, -1, "%d x", b->n);
        break;

    default:
        break;
    }

    al_draw_bitmap(e->cursor, e->msestate.x, e->msestate.y, 0);
}

void game_draw_text(ALLEGRO_FONT *font, ALLEGRO_COLOR color, float x, float y, int flags, const char *str)
{
    float font_offset;

    font_offset = al_get_font_line_height(font)/2;
    al_draw_text(font, al_map_rgb(255, 255, 255), x, y - font_offset, flags, str);
}

void game_draw_num(ALLEGRO_FONT *font, ALLEGRO_COLOR color, float x, float y, int flags, int num)
{
    float font_offset;

    font_offset = al_get_font_line_height(font)/2;
    al_draw_textf(font, al_map_rgb(255, 255, 255), x, y - font_offset, flags, "%d", num);
}

void game_draw_button(ALLEGRO_FONT_STRUCT *fonts, ALLEGRO_COLOR color, box_t pos, const char *str)
{
    ALLEGRO_FONT *f;
    float font_offset;
    vec_t text_pos;

    f = fonts->f22;
    font_offset = al_get_font_line_height(f)/2;
    text_pos = vector_make(pos.x1 + pos.w/2, pos.y1 + pos.h/2 - font_offset);

    al_draw_filled_rounded_rectangle(pos.x1, pos.y1, pos.x2, pos.y2, 20, 20, color);
    al_draw_text(f, al_map_rgb(255, 255, 255), text_pos.x, text_pos.y, -1, str);
}

void game_draw_aim(ball_t *b, float dx, float dy)
{
    float aim_x, aim_y;
    aim_x = b->ini->cx;
    aim_y = b->ini->cy;

    if (dy < -20)
    {
        al_draw_line(aim_x, aim_y, aim_x + dx, aim_y + dy, al_map_rgb(255, 255, 255), 2);
    }
}

void game_draw_pause(engine_t *e, vec_t mouse_pos, box_t gme_b, box_t rsm_b, box_t rst_b, box_t mnu_b)
{
    ALLEGRO_COLOR font_c, rsm_c, rst_c, mnu_c;

    font_c = al_map_rgb(255, 255, 255);
    rsm_c = al_map_rgb(65, 130, 195);
    rst_c = al_map_rgb(234, 35, 94);
    mnu_c = al_map_rgb(0, 163, 150);

    al_draw_filled_rectangle(0, 0, SCR_W, SCR_H, al_map_rgba(0, 0, 0, 150));

    game_draw_text(e->fonts->f30, font_c, gme_b.x2 * 0.5, gme_b.y1 * 1.3, -1, "GAME PAUSED");

    game_draw_button(e->fonts, rsm_c, rsm_b, "RESUME");
    game_draw_button(e->fonts, rst_c, rst_b, "RESTART");
    game_draw_button(e->fonts, mnu_c, mnu_b, "MAIN MENU");

    al_draw_bitmap(e->cursor,mouse_pos.x, mouse_pos.y, 0);
}

void game_draw_over(engine_t *e, game_t *gs, vec_t mouse_pos, box_t rst_b, box_t mnu_b)
{
    ALLEGRO_COLOR font_c, rst_c, mnu_c;

    font_c = al_map_rgb(255, 255, 255);
    rst_c = al_map_rgb(234, 35, 94);
    mnu_c = al_map_rgb(0, 163, 150);

    al_draw_filled_rectangle(0, 0, SCR_W, SCR_H, al_map_rgb(20, 20, 20));

    game_draw_text(e->fonts->f30, font_c, SCR_W*0.5, SCR_H*0.1, -1, "GAME OVER");

    game_draw_text(e->fonts->f18, font_c, SCR_W*0.5, SCR_H*0.24, -1, "SCORE");
    game_draw_num(e->fonts->f60, font_c, SCR_W*0.5, SCR_H*0.3, -1, gs->points);
    
    game_draw_button(e->fonts, rst_c, rst_b, "RESTART");
    game_draw_button(e->fonts, mnu_c, mnu_b, "MAIN MENU");

    al_draw_bitmap(e->cursor,mouse_pos.x, mouse_pos.y, 0);
}

void game_destroy(game_t **gs)
{
    block_matrix_destroy(&(*gs)->m);
    ball_list_destroy(&(*gs)->b);

    free((*gs));
    (*gs) = NULL;
}