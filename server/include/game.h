#ifndef GAME
#define GAME

#define MAX_PLAYER 20
#include <stdint.h>
#include <stdio.h>
#include "list.h"

struct game
{
    uint8_t game_id;
    listElements *joueurs;
    int encours; // -1 notstarted // 0 ingame
    int start;
    int** map;
    listElements *fantomes;
    int heightMap;
    int widthMap;
};
typedef struct game game;

game *get_game(listElements *games,uint8_t game_id); 

#endif
