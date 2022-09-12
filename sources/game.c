#include "game.h"
#include <stdio.h>
#include <stdlib.h>

#define MOVE_BLOCKS 1

game_t *game_make(float scr_w, float scr_h, u_data **d)
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
    if (d != NULL)
        gs->b = ball_list_make(gs->gm_box.x2, gs->gm_box.y2, (*d)->ball);
    else
        gs->b = ball_list_make(gs->gm_box.x2, gs->gm_box.y2, 1);
    if (!gs->m || !gs->b)
    {
        fprintf(stderr, "Falha ao alocar estruturas principais do jogo\n");
        game_destroy(&gs);
        return NULL;
    }

    gs->d = d;
    if (d != NULL)
        gs->high = (*d)->sb_scores[0];

    gs->points = 1;
    if (gs->high == 0)
        gs->high = 1;
    gs->coins = 0;

    gs->scale = 1;

    gs->gst = ANIM;

    return gs;
}

int game_start(engine_t *e, game_t **gs, u_data **d, vec_t *start, vec_t *delta)
{
    if (!(*gs = game_make(SCR_W, SCR_H, d)))
        return 0;

    al_start_timer(e->timer);
    e->last_t = al_get_time();
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

    e->curr_t = al_get_time();
    e->delta_t = (e->curr_t - e->last_t);
    e->last_t = e->curr_t;

    al_flip_display();

    return 1;
}

g_states game_events(engine_t *e, g_states gst, g_states lst, float *scale)
{
    vec_t ms;
    box_t pause_area;

    al_get_mouse_state(&e->msestate);
    al_get_keyboard_state(&e->kbdstate);

    al_wait_for_event(e->queue, &e->event);

    ms = vector_make(e->msestate.x, e->msestate.y);
    pause_area = box_make(5, SCR_H*0.05-SCR_H*0.05/2, SCR_H*0.05, SCR_H*0.05);

    switch (e->event.type)
    {
        case ALLEGRO_EVENT_DISPLAY_CLOSE:
            return STOP;
    }

    if (al_mouse_button_down(&e->msestate, 1) && box_inside(ms, pause_area) && gst != PAUSE)
        return PAUSE;

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
            *scale = 1.5;
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
            e->curr_t = al_get_time();
            e->delta_t = (e->curr_t - e->last_t);
            e->last_t = e->curr_t;
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
            e->curr_t = al_get_time();
            e->delta_t = (e->curr_t - e->last_t);
            e->last_t = e->curr_t;
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
        if (-delta->y > gs->gm_box.h*0.9)
            delta->y = -gs->gm_box.h*0.9;
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
                    ball->cx += (b->first->cx - ball->cx) * 0.1f;
                else if (ball->cx > b->first->cx + 0.5)
                    ball->cx -= (ball->cx - b->first->cx) * 0.1f;
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
            if (*lt >= 0.1 * i || ball->move == true)
            {
                if (ball->move == false)
                    b->n--;
                ball->cx += (ball->sx) * (gs->scale);
                ball->cy += (ball->sy) * (gs->scale);
                ball->move = true;
                
                ball_collide_left(ball, b->radius, gs->gm_box.x1, e->sounds->hit_wall);
                ball_collide_right(ball, b->radius, gs->gm_box.x2, e->sounds->hit_wall);
                ball_collide_top(ball, b->radius, gs->gm_box.y1, e->sounds->hit_wall);
                if (ball_collide_bottom(ball, b->radius, gs->gm_box.y2) && b->l_ctr == b->tam)
                {
                    b->first = ball;
                    ball->move = false;
                    b->l_ctr -= 1;
                }
                ball_collide_block(ball, gs->m, b->radius, nb, &gs->coins, e->sounds);
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
    e->delta_t = 0;
    gs->b->launch = false;
    gs->scale = 1;
    ball_insert(gs->b, *nb);
    *nb = 0;
    gs->gst = ANIM;
    gs->points += 1;
    if (gs->points > gs->high)
        gs->high = gs->points;

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
    bool new_best = false;

    ms_pos = vector_make(e->msestate.x, e->msestate.y);

    game_b = gs->gm_box;
    restart_b = box_make(game_b.x2*0.3, game_b.y2*0.6, game_b.x2*0.4, game_b.y2*0.12);
    menu_b = box_make(game_b.x2*0.3, game_b.y2*0.8, game_b.x2*0.4, game_b.y2*0.12);

    if ((*gs->d) != NULL)
    {
        if ((*gs->d)->sb_scores[0] < gs->high)
        {
            (*gs->d)->sb_scores[0] = gs->high;
            strcpy((*gs->d)->sb_names[0], "LCA");
            new_best = true;
        }
        (*gs->d)->coins += gs->coins;
        gs->coins = 0;

        data_record(*gs->d);
    }

    if (al_mouse_button_down(&e->msestate, 1))
    {
        if (box_inside(ms_pos, restart_b))
            gs->gst = RESTART;
        else if (box_inside(ms_pos, menu_b))
            gs->gst = STOP;
    }
    else
    {
        game_draw_over(e, gs, ms_pos, restart_b, menu_b, new_best);

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
    
    ball_collide_left(b, gs->b->radius, gs->gm_box.x1, e->sounds->hit_wall);
    ball_collide_right(b, gs->b->radius, gs->gm_box.x2, e->sounds->hit_wall);
    ball_collide_top(b, gs->b->radius, gs->gm_box.y1, e->sounds->hit_wall);
    ball_collide_bottom(b, gs->b->radius, gs->gm_box.y2 - gs->b->radius * 2 - 0.5);
    ball_collide_block(b, gs->m, gs->b->radius, nb, &gs->coins, e->sounds);
    b->cx += b->sx;
    b->cy += b->sy;
}

void game_draw(engine_t *e, game_t *gs, vec_t *vector, int anim)
{
    ball_t *b = gs->b;
    box_t g = gs->gm_box;
    matrix_bl *m = gs->m;
    vec_t b_ctr_pos;
    ALLEGRO_BITMAP *pause;
    float p_size;
    double FPS;

    al_clear_to_color(al_map_rgb(30, 30, 30));
    al_draw_filled_rectangle(g.x1, g.y1, g.x2, g.y2, al_map_rgb(20, 20, 20));

    p_size = g.y1*0.5;
    pause = load_bitmap_at_size("resources/models/pause.png", p_size, p_size);
    if (pause)
        al_draw_bitmap(pause, 5, g.y1*0.5-p_size/2, 0);

    draw_text(e->fonts->l10, al_map_rgb(255, 255, 255), SCR_W*0.2, g.y1*0.3, -1, "BEST");
    draw_num(e->fonts->r30, al_map_rgb(255, 255, 255), SCR_W*0.2, g.y1*0.5, -1, gs->high);
    
    draw_num(e->fonts->r30, al_map_rgb(255, 255, 255), SCR_W*0.5, g.y1*0.5, -1, gs->points);

    al_draw_bitmap(gs->m->coin_img, SCR_W*0.87-25/2, g.y1*0.5-25/2, 0);
    if ((*gs->d) != NULL)
    {
        draw_num(e->fonts->r26, al_map_rgb(255, 255, 255), SCR_W*0.8, g.y1*0.5, 1, (*gs->d)->coins);
        if (gs->coins > 0)
            al_draw_textf(e->fonts->r18, al_map_rgb(150,230,150), SCR_W*0.8, g.y1*0.7, 1, "+ %d", gs->coins);
    }
    else
        draw_num(e->fonts->r26, al_map_rgb(255, 255, 255), SCR_W*0.8, g.y1*0.5, 1, gs->coins);

    ball_draw(b);
    block_draw(e, m, anim);

    switch (gs->gst)
    {
    case AIM:
        b_ctr_pos = vector_make(b->ini->cx, b->ini->cy);
        game_draw_aim(b, vector->x, vector->y);
        if (b->n > 0)
            al_draw_textf(e->fonts->r18, al_map_rgb(255, 255, 255), b_ctr_pos.x, b_ctr_pos.y + 10, -1, "%d x", b->n);
        break;

    case LAUNCH:
        if (e->curr_t - gs->b->l_tmr > 6 && gs->scale == 1)
            draw_text(e->fonts->r18, al_map_rgb(120, 120, 120), g.x2*0.97, g.y1*1.3, ALLEGRO_ALIGN_RIGHT, "Press F4 to speed-up");
        b_ctr_pos = vector_make(vector->x, vector->y);
        if (b->n > 0)
            al_draw_textf(e->fonts->r18, al_map_rgb(255, 255, 255), b_ctr_pos.x, b_ctr_pos.y + 10, -1, "%d x", b->n);
        break;

    default:
        break;
    }

    al_draw_bitmap(e->cursor, e->msestate.x, e->msestate.y, 0);

    FPS = 1 / (e->delta_t);
    al_draw_textf(e->fonts->l10, al_map_rgb(50,255,50), 5, 5, 0, "FPS: %d", (int)(FPS));

    al_destroy_bitmap(pause);
}

void game_draw_aim(ball_t *b, float dx, float dy)
{
    float aim_x, aim_y, r;
    int n;
    aim_x = b->ini->cx;
    aim_y = b->ini->cy;
    n = -dy/15;
    r = b->radius*0.9;

    if (dy < -20)
    {
        for (int i=1; i <= n; i++)
        {
            if (n < 5)
                n = 5;
            else if (n >= 15)
                n = 15;
            al_draw_filled_circle(aim_x + (dx/n) * i, aim_y + (dy/n) * i, r, al_map_rgb(220, 220, 220));
        }
    }
}

void game_draw_pause(engine_t *e, vec_t mouse_pos, box_t gme_b, box_t rsm_b, box_t rst_b, box_t mnu_b)
{
    ALLEGRO_COLOR font_c, rsm_c, rst_c, mnu_c;

    font_c = al_map_rgb(255, 255, 255);
    rsm_c = al_map_rgb(65, 130, 195);
    rst_c = al_map_rgb(234, 35, 94);
    mnu_c = al_map_rgb(0, 163, 150);

    al_draw_filled_rectangle(0, 0, SCR_W, SCR_H, al_map_rgba(0, 0, 0, 200));

    draw_text(e->fonts->r40, font_c, SCR_W*0.5, SCR_H*0.2, -1, "GAME PAUSED");

    draw_button(e->fonts, rsm_c, rsm_b, "RESUME", mouse_pos);
    draw_button(e->fonts, rst_c, rst_b, "RESTART", mouse_pos);
    draw_button(e->fonts, mnu_c, mnu_b, "MAIN MENU", mouse_pos);

    al_draw_bitmap(e->cursor,mouse_pos.x, mouse_pos.y, 0);
}

void game_draw_over(engine_t *e, game_t *gs, vec_t mouse_pos, box_t rst_b, box_t mnu_b, bool new_best)
{
    ALLEGRO_COLOR font_c, rst_c, mnu_c;

    font_c = al_map_rgb(255, 255, 255);
    rst_c = al_map_rgb(234, 35, 94);
    mnu_c = al_map_rgb(0, 163, 150);

    al_draw_filled_rectangle(0, 0, SCR_W, SCR_H, al_map_rgb(20, 20, 20));

    draw_text(e->fonts->r40, font_c, SCR_W*0.5, SCR_H*0.2, -1, "GAME OVER");

    if (new_best)
        draw_text(e->fonts->r18, font_c, SCR_W*0.5, SCR_H*0.3, -1, "NEW HIGH SCORE");
    else
        draw_text(e->fonts->r18, font_c, SCR_W*0.5, SCR_H*0.3, -1, "SCORE");
    draw_num(e->fonts->l60, font_c, SCR_W*0.5, SCR_H*0.38, -1, gs->points);
    
    draw_button(e->fonts, rst_c, rst_b, "RESTART", mouse_pos);
    draw_button(e->fonts, mnu_c, mnu_b, "MAIN MENU", mouse_pos);

    al_draw_bitmap(e->cursor,mouse_pos.x, mouse_pos.y, 0);
}

void game_destroy(game_t **gs)
{
    block_matrix_destroy(&(*gs)->m);
    ball_list_destroy(&(*gs)->b);

    free((*gs));
    (*gs) = NULL;
}