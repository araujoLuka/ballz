/* BALLZ TO LINUX - DATA LIBRARY
 *
 * Author: Lucas Araujo
 * Git: https://github.com/araujoLuka/ballz/blob/main/sources/data.h
 * 
 * Describe: Library to get or create user data
*/

#ifndef DATA_H_
#define DATA_H_

// Estrutura com dados do usuario
typedef struct user_data
{
    int coins;
    int ball;
    char sb_names[5][4];
    int sb_scores[5];
} u_data;

// Busca o arquivo com os dados do usuario na pasta do jogo
u_data *data_fetch();

// Cria o arquivo para armazenar os dados de usuario
u_data *data_create();

// Grava informações no arquivo de dados do usuario
void data_record(u_data *d);

// Imprime os dados do usuario
void data_print(u_data *d);

#endif