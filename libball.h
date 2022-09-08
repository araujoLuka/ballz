#ifndef BALL_H_
#define BALL_H_

typedef struct ball
{
    double x, y;
    float sx, sy;
    int move;
    struct ball *next;
} ball_t;

typedef struct ball_list
{
    void *img;
    ball_t *ini;
    int tam;
    int vel;
    int l_ctr;
    float raio;
    int launch;
} list_b;

list_b *make_ball_list(float disp_w, float disp_h);

ball_t *cria_ball(double x, double y);

int insert_ball(list_b *b, int q);

void set_ball_speed(ball_t *b, float x, float y, float vel);

void collide_left(ball_t *b, int lim);

void collide_right(ball_t *b, int lim);

void collide_top(ball_t *b, int lim);

void collide_bottom(ball_t *b, int lim);

list_b *destroy_ball_list(list_b *b);

#endif