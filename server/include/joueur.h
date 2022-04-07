#ifndef PLAYER
#define PLAYER

#include <stdint.h>

struct joueur
{
    char id[8];
    char port[4];
    int x;
    int y;
    uint32_t score;
};
typedef struct joueur joueur;

#endif
