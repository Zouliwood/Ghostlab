#include <entity_position.h>

void player_move(game *game, int direction, joueur joueur, int distance){
    int i;
    switch (direction) {
        case TOP:{
            for (i = 0; i <= distance; ++i) {
                if (joueur.y+i>=game->heightMap || game->map[joueur.y+i][joueur.x]) break;
            }
            joueur.y+=i;
        }
        case RIGHT:{
            for (i = 0; i <= distance; ++i) {
                if (joueur.x+i>=game->widthMap || game->map[joueur.y][joueur.x+i]) break;
            }
            joueur.x+=i;
        }
        case BOTTOM:{
            for (i = 0; i <= distance; ++i) {
                if (joueur.y-i<=0 || game->map[joueur.y-i][joueur.x]) break;
            }
            joueur.y-=i;
        }
        default:{ //LEFT
            for (i = 0; i <= distance; ++i) {
                if (joueur.x-i<=0 || game->map[joueur.y][joueur.x-i]) break;
            }
            joueur.x-=i;
        }
    }
}

void generic_move(void* entity, int isGhost, game game){
    int new_x, new_y;
    do{
        new_x=(rand()%(game.widthMap+1));
        new_y=(rand()%(game.heightMap+1));
    }while (!game.map[new_y][new_x]);

    if (isGhost){
        ((fantome *)entity)->x=new_x;
        ((fantome *)entity)->y=new_y;
    }else{
        ((joueur *)entity)->x=new_x;
        ((joueur *)entity)->y=new_y;
    }
}

void ghost_move(fantome fantome, game game){
    generic_move(&fantome, 1, game);
}

void init_pos_player(joueur joueur, game game){
    generic_move(&joueur, 0, game);
}

void init_ghost_move(fantome fantome, game game){
    generic_move(&fantome, 1, game);
}