#ifndef GHOSTLAB_FANTOME_H
#define GHOSTLAB_FANTOME_H

#include <stdint.h>
#include "game.h"

struct fantome
{
    int x;
    int y;
    int score;
    //TODO: add malus/bonus?
};
typedef struct fantome fantome;

#endif //GHOSTLAB_FANTOME_H
