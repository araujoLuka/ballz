#ifndef OBJECT_H_
#define OBJECT_H_

typedef struct vetor
{
    float x;
    float y;
} vet_t;

typedef struct box
{
    float x;
    float y;
    float w;
    float h;
} box_t;

vet_t vet_make(float x, float y);

vet_t *vet_destroy(vet_t *vetor);

vet_t vet_dif(vet_t v1, vet_t v2);


box_t *box_make(float x, float y, float w, float h);

box_t *box_destroy(box_t *box);


int box_inside (vet_t vetor, box_t obj);

#endif