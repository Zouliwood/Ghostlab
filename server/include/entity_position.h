#ifndef GHOSTLAB_ENTITY_POSITION_H
#define GHOSTLAB_ENTITY_POSITION_H

#include <stdlib.h>

#include "game.h"
#include "joueur.h"
#include "fantome.h"

#define TOP 0
#define RIGHT 1
#define BOTTOM 2
#define LEFT 3

void player_move(game game, int direction, joueur* joueur, int distance);
void ghost_move(fantome* fantome, game game);
void init_pos_player(joueur* joueur, game game);
void init_ghost_move(fantome* fantome, game game);

#endif //GHOSTLAB_ENTITY_POSITION_H
