#include "game.h"

game *get_game(listElements *games, uint8_t game_id)
{
    if (games == NULL || games->first == NULL)
        return NULL;
    element *ptr = games->first;
    while (ptr)
    {
        if (((game *)ptr->data)->game_id == game_id)
        {
            return ptr->data;
        }
        else
        {
            ptr = ptr->next;
        }
    }
    return NULL;
}

void *freeGame(game *ptr)
{
    free(ptr->joueurs);
    free(ptr->fantomes);
    freeMap(ptr->map, ptr->heightMap);
    free(ptr);
    return NULL;
}

void *initGhost(game *game_current)
{
    if (game_current == NULL || game_current->fantomes == NULL)
        return NULL;
    fantome *ghost = malloc(sizeof(fantome));
    ghost->score = 15;
    init_ghost_move(ghost, game_current);
    addEl(game_current->fantomes, game_current->fantomes->first, ghost);
    element *ptr = game_current->fantomes->first;
    for (int i = 0; i < 19; i++)
    {
        fantome *g = malloc(sizeof(fantome));
        g->score = 15 * (i % 4 + 1);
        init_ghost_move(g, game_current);
        ptr = addEl(game_current->fantomes, ptr, g);
    }
    return NULL;
}
