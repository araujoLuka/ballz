#ifndef DATA_H_
#define DATA_H_

typedef struct user_data
{
    int coins;
    int ball;
    char sb_names[5][4];
    int sb_scores[5];
} u_data;

u_data *data_fetch();

u_data *data_create();

void data_record(u_data *d);

void data_print(u_data *d);

#endif