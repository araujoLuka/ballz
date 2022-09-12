#include "data.h"
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>

#define BUFFSIZE 50

u_data *data_fetch()
{
    DIR *dir;
    FILE *file;
    struct dirent *f;
    u_data *d;
    char buff[BUFFSIZE+1];

    if (!(dir = opendir("./")))
    {
        fprintf(stderr, "Falha ao buscar dados de save\n");
        return NULL;
    }

    while (1)
    {
        if (!(f = readdir(dir)))
            return NULL;
        if (strstr(f->d_name, ".sve"))
        {
            strcpy(buff, "./");
            strcat(buff, f->d_name);
            file = fopen(buff, "rb+");
            if (file == NULL)
                return NULL;
            d = malloc(sizeof(u_data));

            fread(d, sizeof(u_data), 1, file);

            break;
        }
    }

    return d;
}

u_data *data_create()
{
    FILE *file;
    u_data *d;

    file = fopen(".udata.sve", "wb");
    if (!file)
        return NULL;

    d = malloc(sizeof(u_data));
    if (!d)
        return NULL;

    d->ball = 1;
    d->coins = 0;
    for (int i=0; i < 5; i++)
    {
        strcpy(d->sb_names[i], "---\0");
        d->sb_scores[i] = 0;
    }

    fclose(file);
    data_record(d);

    return d;
}

void data_record(u_data *d)
{
    FILE *file;

    file = fopen(".udata.sve", "wb");
    fwrite(d, sizeof(u_data), 1, file);
}

void data_print(u_data *d)
{
    printf("%d\n", d->ball);
    printf("%d\n", d->coins);
    for(int i = 0; i < 5; i++)
    {
        printf("%s ", d->sb_names[i]);
        printf("%d\n", d->sb_scores[i]);
    }
}