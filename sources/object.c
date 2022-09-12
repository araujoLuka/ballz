#include "object.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

vec_t vector_make(float x, float y)
{
    vec_t v;

    v.x = x;
    v.y = y;

    return v;
}

vec_t *vector_destroy(vec_t *vetor)
{
    free(vetor);
    return NULL;
}

float vector_len(vec_t v)
{
    double square_sum;
    square_sum = v.x*v.x + v.y*v.y;
    if (square_sum < 0)
        return -sqrt(-square_sum);

    return sqrt(square_sum);
}

float vector_lenSq(vec_t v)
{
    float square_sum;
    square_sum = v.x*v.x + v.y*v.y;

    return square_sum;
}

vec_t vector_norm(vec_t v)
{
    vec_t n;
    float l = vector_len(v);

    if (l == 0)
        return v;
    
    n.x = (v.x / l);
    n.y = (v.y / l);

    return n;
}

vec_t vector_dif(vec_t v1, vec_t v2)
{
    return vector_make(v1.x - v2.x, v1.y - v2.y);
}

void vector_print(vec_t v)
{
    printf("(%f, %f)\n", v.x, v.y);
}

box_t box_make(float px, float py, float width, float height)
{
    box_t b;

    b.x1 = px;
    b.y1 = py;
    b.x2 = px + width;
    b.y2 = py + height;
    b.w = width;
    b.h = height;

    return b;
}

circ_t circle_make(float x, float y, float radius)
{
    circ_t c;

    c.x = x;
    c.y = y;
    c.radius = radius;

    return c;
}

int box_inside (vec_t point, box_t obj)
{
    int hz, vt;
    hz = 0;
    vt = 0;

    if (point.x >= obj.x1 && point.x <= obj.x2)
        hz = 1;
    
    if (point.y >= obj.y1 && point.y <= obj.y2)
        vt = 1;

    if (hz && vt)
        return 1;

    return 0;
}

int circle_inside (vec_t point, circ_t obj)
{
    vec_t dist;
    dist = vector_dif(vector_make(obj.x, obj.y), point);

    if (vector_len(dist) < obj.radius)
        return 1;
    
    return 0;
}