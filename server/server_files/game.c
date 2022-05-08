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
        int r=rand()%30;
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
            ghost_move((fantome*)ptr->data, game_current);
            ptr = ptr->next;
        }
    }
}
