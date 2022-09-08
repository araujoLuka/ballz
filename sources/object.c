#include "object.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

vet_t vet_make(float x, float y)
{
    vet_t v;

    v.x = x;
    v.y = y;

    return v;
}

vet_t *vet_destroy(vet_t *vetor)
{
    free(vetor);
    return NULL;
}

float vet_len(vet_t v)
{
    return sqrt(v.x*v.x + v.y*v.y);
}

vet_t vet_dif(vet_t v1, vet_t v2)
{
    return vet_make(v1.x - v2.x, v1.y - v2.y);
}

box_t *box_make(float px, float py, float width, float height)
{
    box_t *b;

    b = malloc(sizeof(box_t));
    if (!b)
    {
        fprintf(stderr, "Falha ao alocar objeto box\n");
        return NULL;
    }

    b->x = px;
    b->y = py;
    b->w = width;
    b->h = height;

    return b;
}

box_t *box_destroy(box_t *box)
{
    free(box);
    return NULL;
}

int box_inside (vet_t vetor, box_t obj)
{
    int hz, vt;
    hz = 0;
    vt = 0;

    if (vetor.x >= obj.x && vetor.x <= obj.w)
        hz = 1;
    
    if (vetor.y >= obj.y && vetor.y <= obj.h)
        vt = 1;

    if (hz && vt)
        return 1;

    return 0;
}