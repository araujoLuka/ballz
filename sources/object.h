/* BALLZ TO LINUX - OBJECT LIBRARY
 *
 * Author: Lucas Araujo
 * Git: https://github.com/araujoLuka/ballz/blob/main/sources/object.h
 * 
 * Describe: Library that controls game objects (vectors, boxes, circles)
*/

#ifndef OBJECT_H_
#define OBJECT_H_

// Estrutura para vetores
typedef struct vetor
{
    float x;
    float y;
} vec_t;

// Estrutura para caixas
typedef struct box
{
    float x1, y1;
    float x2, y2;
    float w, h;
} box_t;

// Estrutura para circulos
typedef struct circle
{
    float x, y;
    float radius;
} circ_t;

// Cria um vetor
vec_t vector_make(float x, float y);

// Libera memoria de um vetor alocado dinamicamente
vec_t *vector_destroy(vec_t *vetor);

// Realiza a subtração entre o vetor 1 e o vetor 2
vec_t vector_dif(vec_t v1, vec_t v2);

// Calcula o tamanho de um vetor
float vector_len(vec_t v);

// Calcula o tamanho ao quadrado de um vetor
float vector_lenSq(vec_t v);

// Normaliza um vetor
vec_t vector_norm(vec_t v);

// Imprime os dados de um vetor
void vector_print(vec_t v);

// Cria uma caixa
box_t box_make(float x, float y, float w, float h);

// Libera memoria de uma caixa alocada dinamicamente
box_t *box_destroy(box_t *box);

// Cria um circulo
circ_t circle_make(float x, float y, float radius);

// Verifica se um ponto está dentro de uma caixa
int box_inside (vec_t point, box_t obj);

// Verifica se um ponto está dentro de uma circulo
int circle_inside (vec_t point, circ_t obj);

#endif