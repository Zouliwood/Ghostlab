#include <entity_position.h>
#include <pthread.h>

extern pthread_mutex_t verrou;

void check_ghost(game *game, joueur *joueur, int sock)
{
    pthread_mutex_lock(&verrou);
    element *el = game->fantomes->first;
    pthread_mutex_unlock(&verrou);
    while (el != NULL)
    {
        if (((fantome *)el->data)->x == joueur->x && ((fantome *)el->data)->y == joueur->y)
        {
            pthread_mutex_lock(&verrou);
            joueur->score += ((fantome *)el->data)->score;
            pthread_mutex_unlock(&verrou);
            // creation message udp
            int size_mess = SIZE_OF_HEAD + SIZE_OF_END + 8 + 3 + 3 + 4 + 4;
            char fantome_taken[size_mess + 1];

            memmove(fantome_taken, "SCORE", SIZE_OF_HEAD);
            memmove(fantome_taken + SIZE_OF_HEAD, " ", 1);
            memmove(fantome_taken + SIZE_OF_HEAD + 1, joueur->id, 8);
            memmove(fantome_taken + SIZE_OF_HEAD + 9, " ", 1);
            sprintf(fantome_taken + SIZE_OF_HEAD + 10, "%04d", joueur->score);
            memmove(fantome_taken + SIZE_OF_HEAD + 14, " ", 1);
            sprintf(fantome_taken + SIZE_OF_HEAD + 15, "%03d", joueur->x);
            memmove(fantome_taken + SIZE_OF_HEAD + 18, " ", 1);
            sprintf(fantome_taken + SIZE_OF_HEAD + 19, "%03d", joueur->y);
            memmove(fantome_taken + SIZE_OF_HEAD + 22, "+++", SIZE_OF_END);
            fantome_taken[size_mess] = '\0';
            sendMulticast(joueur->current, fantome_taken,size_mess);
            free((fantome *)el->data);
            removeEl(game->fantomes, el);
            break; // 1 fantome par case au plus
        }
        pthread_mutex_lock(&verrou);
        el = el->next;
        pthread_mutex_unlock(&verrou);
    }
}

void player_move(game *game, int direction, joueur *joueur, int distance, int sock)
{
    switch (direction)
    {
    case 0:
        while (distance > 0)
        {
            if (joueur->y + 1 >= game->heightMap || game->map[joueur->y + 1][joueur->x])
                break;
            else
            {
                pthread_mutex_lock(&verrou);
                joueur->y += 1;
                pthread_mutex_unlock(&verrou);
                check_ghost(game, joueur, sock);
            }
            distance--;
        }
        break;
    case 1:
        while (distance > 0)
        {
            if (joueur->x + 1 >= game->widthMap || game->map[joueur->y][joueur->x + 1])
                break;
            else
            {
                pthread_mutex_lock(&verrou);
                joueur->x += 1;
                pthread_mutex_unlock(&verrou);
                check_ghost(game, joueur, sock);
            }
            distance--;
        }
        break;
    case 2:
        while (distance > 0)
        {
            if (joueur->y - 1 < 0 || game->map[joueur->y - 1][joueur->x])
                break;
            else
            {
                pthread_mutex_lock(&verrou);
                joueur->y -= 1;
                pthread_mutex_unlock(&verrou);
                check_ghost(game, joueur, sock);
            }
            distance--;
        }
        break;
    case 3:
        while (distance > 0)
        {
            if (joueur->x - 1 < 0 || game->map[joueur->y][joueur->x - 1])
                break;
            else
            {
                pthread_mutex_lock(&verrou);
                joueur->x -= 1;
                pthread_mutex_unlock(&verrou);
                check_ghost(game, joueur, sock);
            }
            distance--;
        }
        break;
    }
}

void generic_move(void *entity, int isGhost, game *game)
{
    int new_x, new_y;
    do
    {
        new_x = (rand() % (game->widthMap));
        new_y = (rand() % (game->heightMap));
    } while (game->map[new_y][new_x]);

    if (isGhost)
    {
        ((fantome *)entity)->x = new_x;
        ((fantome *)entity)->y = new_y;
    }
    else
    {
        ((joueur *)entity)->x = new_x;
        ((joueur *)entity)->y = new_y;
    }
}

/**
 * AVANT UTILISATION: Vérifier que le nombre de cases vides est suffisant.
 * @param fantome
 * @param game
 */
void init_pos_player(joueur *player, game *game)
{
    generic_move(player, 0, game);
    element *el = game->fantomes->first;
    while (el != NULL)
    {
        if (((fantome *)el->data)->x == player->x && ((fantome *)el->data)->y == player->y)
        {
            init_pos_player(player, game);
        }
        el = el->next;
    }
    pthread_mutex_lock(&verrou);
    el = game->joueurs->first;
    pthread_mutex_unlock(&verrou);
    while (el != NULL)
    {
        if (((joueur *)el->data)->x == player->x && ((joueur *)el->data)->y == player->y && player != ((joueur *)el->data))
        {
            init_pos_player(player, game);
        }
        pthread_mutex_lock(&verrou);
        el = el->next;
        pthread_mutex_unlock(&verrou);
    }
}

void no_collision(void (*mov)(fantome *fantome, game *game), fantome *fantom, game *game)
{
    pthread_mutex_lock(&verrou);
    element *el = game->joueurs->first;
    pthread_mutex_unlock(&verrou);
    while (el != NULL)
    {
        if (((joueur *)el->data)->x == fantom->x && ((joueur *)el->data)->y == fantom->y)
        {
            mov(fantom, game);
        }
        pthread_mutex_lock(&verrou);
        el = el->next;
        pthread_mutex_unlock(&verrou);
    }
    pthread_mutex_lock(&verrou);
    el = game->fantomes->first;
    pthread_mutex_unlock(&verrou);
    while (el != NULL)
    {
        if (((fantome *)el->data)->x == fantom->x && ((fantome *)el->data)->y == fantom->y && fantom != ((fantome *)el->data))
        {
            mov(fantom, game);
        }
        pthread_mutex_lock(&verrou);
        el = el->next;
        pthread_mutex_unlock(&verrou);
    }
}

/**
 * AVANT UTILISATION: Vérifier que le nombre de cases vides est suffisant.
 * @param fantome
 * @param game
 */
void ghost_move(fantome *fantome, game *game)
{
    srand(time(NULL));
    int direction = rand() % 4;
    switch (direction)
    {
    case 0:
        if (fantome->y + 1 >= game->heightMap || game->map[fantome->y + 1][fantome->x])
        {
            break;
        }
        else
        {
            pthread_mutex_lock(&verrou);
            fantome->y += 1;
            pthread_mutex_unlock(&verrou);
        }
        break;
    case 1:
        if (fantome->x + 1 >= game->widthMap || game->map[fantome->y][fantome->x + 1])
            break;
        else
        {
            pthread_mutex_lock(&verrou);
            fantome->x += 1;
            pthread_mutex_unlock(&verrou);
        }
        break;
    case 2:
        if (fantome->y - 1 < 0 || game->map[fantome->y - 1][fantome->x])
            break;
        else
        {
            pthread_mutex_lock(&verrou);
            fantome->y -= 1;
            pthread_mutex_unlock(&verrou);
        }
        break;
    case 3:
        if (fantome->x - 1 < 0 || game->map[fantome->y][fantome->x - 1])
            break;
        else
        {
            pthread_mutex_lock(&verrou);
            fantome->x -= 1;
            pthread_mutex_unlock(&verrou);
        }
        break;
    }
    no_collision(ghost_move, fantome, game);
}

/**
 * AVANT UTILISATION: Vérifier que le nombre de cases vides est suffisant.
 * @param fantome
 * @param game
 */
void init_ghost_move(fantome *fantome, game *game)
{
    generic_move(fantome, 1, game);
    no_collision(init_ghost_move, fantome, game);
}