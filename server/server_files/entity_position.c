#include <entity_position.h>
#include <pthread.h>

//TODO: mutex with struct
//pthread_mutex_t verrou= PTHREAD_MUTEX_INITIALIZER;

void check_ghost(game *game, joueur* joueur, int i){
    element* el=game->fantomes->first;
    while (el!=NULL){
        if ((*(struct fantome *)el).x == joueur->x && (*(struct fantome*)el).y == joueur->y+i){
            joueur->score+=(*(struct fantome *)el).score;
            //TODO: UDP
            removeEl(game->fantomes, el);
            free(el);
            break;//1 fantome par case au plus
        }
        el=el->next;
    }
}

void player_move(game *game, int direction, joueur* joueur, int distance){
    int i;
    //pthread_mutex_lock(&verrou);
    switch (direction) {
        case 0:
            printf("0\n");
            for (i = 0; i <= distance; ++i) {
                printf("%d cond 1; %d cond 2\n",joueur->y+i,game->map[joueur->y+i][joueur->x]);
                if (joueur->y+i>=game->heightMap || game->map[joueur->y+i][joueur->x]) break;
                else check_ghost(game, joueur, i);
            }
            printf("%d\n",i);
            joueur->y+=i;
            break;
        case 1:
            printf("1\n");
            for (i = 0; i <= distance; ++i) {
                printf("%d cond 1; %d cond 2\n",joueur->x+i,game->map[joueur->y][joueur->x+i]);
                if (joueur->x+i>=game->widthMap || game->map[joueur->y][joueur->x+i]) break;
                else check_ghost(game, joueur, i);
            }
            printf("%d\n",i);
            joueur->x+=i;
            break;
        case 2:
            printf("2\n");
            for (i = 0; i <= distance; ++i) {
                printf("%d cond 1; %d cond 2\n",joueur->y-i,game->map[joueur->y-i][joueur->x]);
                if (joueur->y-i<=0 || game->map[joueur->y-i][joueur->x]) break;
                else check_ghost(game, joueur, i);
            }
            printf("%d\n",i);
            joueur->y-=i;
            break;
        case 3:
            printf("3\n");
            for (i = 0; i <= distance; ++i) {
                printf("%d cond 1; %d cond 2\n",joueur->x-i,game->map[joueur->y][joueur->x-i]);
                if (joueur->x-i<=0 || game->map[joueur->y][joueur->x-i]) break;
                else check_ghost(game, joueur, i);
            }
            printf("%d\n",i);
            joueur->x-=i;
            break;
    }
    //pthread_mutex_unlock(&verrou);
}

void generic_move(void* entity, int isGhost, game *game){
    int new_x, new_y;
    //pthread_mutex_lock(&verrou);
    do{
        new_x=(rand()%(game->widthMap));
        new_y=(rand()%(game->heightMap));
    }while (game->map[new_y][new_x]);

    if (isGhost){
        ((fantome *)entity)->x=new_x;
        ((fantome *)entity)->y=new_y;
    }else{
        ((joueur *)entity)->x=new_x;
        ((joueur *)entity)->y=new_y;
    }
    //pthread_mutex_unlock(&verrou);
}

/**
 * AVANT UTILISATION: Vérifier que le nombre de cases vides est suffisant.
 * @param fantome
 * @param game
 */
void init_pos_player(joueur* player, game *game){
    generic_move(player, 0, game);
    //pthread_mutex_lock(&verrou);
    //éviter les collisions
    element* el=game->fantomes->first;
    while (el!=NULL){
        if (((fantome *)el->data)->x == player->x && ((fantome*)el->data)->y == player->y){
            init_pos_player(player, game);
        }
        el=el->next;
    }
    el=game->joueurs->first;
    while (el!=NULL){
        if (((joueur*)el->data)->x == player->x && ((joueur*)el->data)->y == player->y
        && player!=((joueur *)el->data)){
            init_pos_player(player, game);
        }
        el=el->next;
    }
    //pthread_mutex_unlock(&verrou);
}

void no_collision(void (*mov)(fantome* fantome,game *game), fantome* fantom, game *game){
    //pthread_mutex_lock(&verrou);
    element* el=game->joueurs->first;
    while (el!=NULL){
        if (((joueur*)el->data)->x == fantom->x && ((joueur*)el->data)->y == fantom->y){
            mov(fantom, game);
        }
        el=el->next;
    }

    el=game->fantomes->first;
    while (el!=NULL){
        if (((fantome *)el->data)->x == fantom->x && ((fantome*)el->data)->y == fantom->y
        && fantom!=((fantome *)el->data)){
            mov(fantom, game);
        }
        el=el->next;
    }
    //pthread_mutex_unlock(&verrou);
}

/**
 * AVANT UTILISATION: Vérifier que le nombre de cases vides est suffisant.
 * @param fantome
 * @param game
 */
void ghost_move(fantome* fantome, game *game){
    generic_move(fantome, 1, game);
    no_collision(ghost_move, fantome, game);
}

/**
 * AVANT UTILISATION: Vérifier que le nombre de cases vides est suffisant.
 * @param fantome
 * @param game
 */
void init_ghost_move(fantome* fantome, game *game){
    generic_move(fantome, 1, game);
    no_collision(init_ghost_move, fantome, game);
}