#include "game.h"

extern pthread_mutex_t verrou;

game *get_game(listElements *games, uint8_t game_id)
{
    if (games == NULL || games->first == NULL)
        return NULL;
    pthread_mutex_lock(&verrou);
    element *ptr = games->first;
    pthread_mutex_unlock(&verrou);
    while (ptr)
    {
        if (((game *)ptr->data)->game_id == game_id)
        {
            return ptr->data;
        }
        else
        {
            pthread_mutex_lock(&verrou);
            ptr = ptr->next;
            pthread_mutex_unlock(&verrou);
        }
    }
    return NULL;
}

void freeGame(game *ptr)
{
    free(ptr->joueurs);
    free(ptr->fantomes);
    freeMap(ptr->map, ptr->heightMap);
    free(ptr);
}

void initGhost(game *game_current)
{
    if (game_current == NULL || game_current->fantomes == NULL)
    {
        printf("Error while init ghost\n");
    }
    else
    {
        fantome *ghost = malloc(sizeof(fantome));
        ghost->score = 15;
        init_ghost_move(ghost, game_current);
        addEl(game_current->fantomes, game_current->fantomes->first, ghost);
        element *ptr = game_current->fantomes->first;
        srand(time(NULL));
        int r=rand()%10;
        for (int i = 0; i < r; i++)
        {
            fantome *g = malloc(sizeof(fantome));
            g->score = 15 * (i % 4 + 1);
            init_ghost_move(g, game_current);
            ptr = addEl(game_current->fantomes, ptr, g);
        }
    }
}

void movGhost(game *game_current)
{
    if (game_current == NULL || game_current->fantomes == NULL)
    {
        printf("Error while moving ghost\n");
    }
    else
    {
        element *ptr = game_current->fantomes->first;
        for (int i = 0; i < game_current->fantomes->count; i++)
        {
            fantome *current =(fantome*)ptr->data; 
            int x=current->x;
            int y=current->y;
            ghost_move(current, game_current);
            if(x!=current->x || y!=current->y)
            {
                int size=SIZE_OF_HEAD+SIZE_OF_END+3+3+2;
                char ghost_mov[size+1];
                memmove(ghost_mov,"GHOST",SIZE_OF_HEAD);
                memmove(ghost_mov+SIZE_OF_HEAD," ",1);
                sprintf(ghost_mov+SIZE_OF_HEAD+1,"%03d",current->x);
                memmove(ghost_mov+SIZE_OF_HEAD+4," ",1);
                sprintf(ghost_mov+SIZE_OF_HEAD+5,"%03d",current->y);
                memmove(ghost_mov+SIZE_OF_HEAD+8,"+++",3);
                ghost_mov[size]='\0';
                sendMulticast(game_current,ghost_mov,size);
            }
            pthread_mutex_lock(&verrou);
            ptr = ptr->next;
            pthread_mutex_unlock(&verrou);
        }
    }
}

joueur *getWinner(game *current)
{
    if(current->joueurs->count>1)
    {
        pthread_mutex_lock(&verrou);
        element *max=current->joueurs->first;
        element *ptr=current->joueurs->first->next;
        pthread_mutex_unlock(&verrou);
        for(int i =0;i <getListCount(current->joueurs)-1;i++)
        {
            if(((joueur *)ptr->data)->score>((joueur *)max->data)->score)
            {
                max=ptr;
            }
            pthread_mutex_lock(&verrou);
            ptr=ptr->next;
            pthread_mutex_unlock(&verrou);
        }
        return (joueur *)max->data;

    }else
    {
        return (joueur *)current->joueurs->first->data;
    }
}