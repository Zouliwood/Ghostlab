#ifndef GAME
#define GAME

#define MAX_PLAYER 20
#include <stdint.h>
#include <stdio.h>
#include "list.h"
#include "labyrinthe.h"
#include "fantome.h"


struct game
{
    uint8_t game_id;
    listElements *joueurs;
    int encours; // -1 notstarted // 1 ingame
    int start;
    int** map;
    listElements *fantomes;
    uint16_t heightMap;
    uint16_t widthMap;
};
typedef struct game game;

#include "entity_position.h"

game *get_game(listElements *games,uint8_t game_id);
void freeGame(game *ptr);
void initGhost(game *game_current);
void movGhost(game *game_current);

#endif
