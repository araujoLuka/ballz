#ifndef BALL_H_
#define BALL_H_

typedef struct ball
{
    double cx, cy;
    double sx, sy;
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

list_b *ball_list_make(float disp_w, float disp_h);

ball_t *ball_make(double x, double y);

int ball_insert(list_b *b, int q);

void ball_launch(list_b *b, float x, float y, float vel);

int ball_collide_left(ball_t *b, float r, int lim);

int ball_collide_right(ball_t *b, float r, int lim);

int ball_collide_top(ball_t *b, float r, int lim);

int ball_collide_bottom(ball_t *b, float r, int lim);

int ball_collide_block(ball_t *b, void *ptr_m, float r, int *nb, int *c);

void ball_list_destroy(list_b **b);

#endif