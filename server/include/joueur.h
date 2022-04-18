#ifndef PLAYER
#define PLAYER

#include <stdint.h>
#include "game.h"

struct joueur
{
    char id[8];
    char port[4];
    int x;
    int y;
    uint32_t score;
    game *current;
    int game_status;
};
typedef struct joueur joueur;

#endif
