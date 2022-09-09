#ifndef OBJECT_H_
#define OBJECT_H_

typedef struct vetor
{
    float x;
    float y;
} vec_t;

typedef struct box
{
    float x1, y1;
    float x2, y2;
    float w, h;
} box_t;

vec_t vector_make(float x, float y);

vec_t *vector_destroy(vec_t *vetor);

vec_t vector_dif(vec_t v1, vec_t v2);

float vector_len(vec_t v);

vec_t vector_norm(vec_t v);

void vector_print(vec_t v);


box_t box_make(float x, float y, float w, float h);

box_t *box_destroy(box_t *box);


int box_inside (vec_t vetor, box_t obj);

#endif