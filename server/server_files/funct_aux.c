#include "funct_aux.h"

void* func_send_games(int sock2, listElements *games)
{
    int size_games = sizeof(uint8_t) + SIZE_OF_HEAD + SIZE_OF_END + 1;
    char games_mess[size_games];
    memmove(games_mess, GAMES, SIZE_OF_HEAD);
    memmove(games_mess + SIZE_OF_HEAD, " ", 1);
    memmove(games_mess + SIZE_OF_HEAD + 1, &(games->count), sizeof(uint8_t));
    memmove(games_mess + SIZE_OF_HEAD + 1 + sizeof(uint8_t), END_TCP, SIZE_OF_END);
    if (size_games != send(sock2, games_mess, size_games, 0))
    {
        printf("Couldn't send GAMES");
    }

    element *ptr = games->first;
    for (int i = 0; i < games->count; i++)
    {
        uint8_t gamed_id = ((game *)ptr->data)->game_id;
        printf("%d\n",gamed_id);
        uint8_t player_count = ((game *)ptr->data)->joueurs->count;
        printf("%d\n",player_count);
        int taille = (sizeof(uint8_t) * 2) + SIZE_OF_HEAD + 2 + SIZE_OF_END;
        char game_mess[taille];
        memmove(game_mess, OGAME, SIZE_OF_HEAD);
        memmove(game_mess + SIZE_OF_HEAD, " ", 1);
        memmove(game_mess + SIZE_OF_HEAD + 1, &gamed_id, sizeof(uint8_t));
        memmove(game_mess + SIZE_OF_HEAD + 1 + sizeof(uint8_t), " ", 1);
        memmove(game_mess + SIZE_OF_HEAD + 2 + sizeof(uint8_t),&player_count, sizeof(uint8_t));
        memmove(game_mess + SIZE_OF_HEAD + 2 + (sizeof(uint8_t) * 2), END_TCP, SIZE_OF_END);
        if (taille != send(sock2, game_mess, taille, 0))
        {
            printf("Couldn't send OGAME");
        }
        ptr = ptr->next;
    }
    return NULL;
}

joueur *new_game(int sock2, listElements *games)
{
    // recv suite de NEWPL_id_port***
    int taille = 14 + SIZE_OF_END ;
    char buffer[taille];
    if (taille != recv(sock2, buffer, taille, 0))
    {
        func_send_dunno(sock2);
        return NULL;
    }
    printf("creation du joueur\n");
    // création du joueur
    joueur *j1 = malloc(sizeof(joueur));
    memmove(j1->id, buffer + 1, 8);
    memmove(j1->port, buffer + 10, 4);
    j1->score = 0;
    j1->game_status = 0;
    j1->x = -1;
    j1->y = -1;
    printf("creation de la partie\n");
    // création de la partie
    game *new = malloc(sizeof(game));
    new->encours = -1;
    new->start = 0;
    new->game_id = games->count + 1;
    new->joueurs = malloc(sizeof(listElements));
    addEl(new->joueurs,NULL, j1);
    addEl(games, games->last, new);
    j1->current = new;
    printf("envoie du message\n");
    char message[SIZE_OF_END + SIZE_OF_HEAD];
    memmove(message, REGOK, SIZE_OF_HEAD);
    memmove(message + SIZE_OF_HEAD, END_TCP, SIZE_OF_END);
    if ((SIZE_OF_END + SIZE_OF_HEAD) != send(sock2, message, SIZE_OF_END + SIZE_OF_HEAD, 0))
    {
        printf("couldn't send regok");
    }
    return j1;
}

joueur *register_game(int sock2, listElements *games)
{
    // recv suite de REGIS_id_port_m***
    int taille = 3 + SIZE_OF_END + 8 + 4 + sizeof(uint8_t);
    char buffer[taille];
    if (taille != recv(sock2, buffer, taille, 0))
    {
        func_send_dunno(sock2);
        return NULL;
    }
    uint8_t id = *(uint8_t *)(buffer + 15);
    game *_game = get_game(games, id);
    if (!_game)
    {
        func_send_regno(sock2);
        return NULL;
    }
    // création du joueur
    joueur *j1 = malloc(sizeof(joueur));
    memmove(j1->id, buffer + 1, 8);
    memmove(j1->port, buffer + 10, 4);
    j1->score = 0;
    j1->x = -1;
    j1->y = -1;
    addEl(_game->joueurs, _game->joueurs->last, j1);
    j1->current = _game;

    char message[SIZE_OF_END + SIZE_OF_HEAD];
    memmove(message, REGOK, SIZE_OF_HEAD);
    memmove(message + SIZE_OF_HEAD, END_TCP, SIZE_OF_END);
    if ((SIZE_OF_END + SIZE_OF_HEAD) != send(sock2, message, SIZE_OF_END + SIZE_OF_HEAD, 0))
    {
        printf("couldn't send regok");
        return NULL;
    }
    return j1;
}

void *func_send_dunno(int sock2)
{
    int taille = SIZE_OF_HEAD + SIZE_OF_END;
    char message[taille];
    memmove(message, DUNNO, SIZE_OF_HEAD);
    memmove(message + SIZE_OF_HEAD, END_TCP, SIZE_OF_END);
    int count = send(sock2, message, taille, 0);
    if (count != taille)
        printf("Couldn't send duno");
    return NULL;
}

void *func_send_regno(int sock2)
{
    int taille = SIZE_OF_HEAD + SIZE_OF_END;
    char message[taille];
    memmove(message, REGNO, SIZE_OF_HEAD);
    memmove(message + SIZE_OF_HEAD, END_TCP, SIZE_OF_END);
    int count = send(sock2, message, taille, 0);
    if (count != taille)
        printf("Couldn't send regno");
    return NULL;
}

void *start_game(joueur *joueur, int socket)
{
    if (joueur->current == NULL)
    {
        func_send_dunno(socket);
        return NULL;
    }
    joueur->current->start += 1;
    if (joueur->current->start > 1 && joueur->current->start == joueur->current->joueurs->count)
    {
        // démarage de la partie
    }
    return NULL;
}

void *func_unreg(joueur *joueur, int sock)
{
    // remove joueur from game and free mem
    element *ptr = joueur->current->joueurs->first;
    while (ptr->data != joueur)
    {
        ptr = ptr->next;
    }
    removeEl(joueur->current->joueurs, ptr);
    free(joueur);

    // send unrok
    int taille = SIZE_OF_HEAD + SIZE_OF_END;
    char message[taille];
    memmove(message, UNROK, SIZE_OF_HEAD);
    memmove(message + SIZE_OF_HEAD, END_TCP, SIZE_OF_END);
    int count = send(sock, message, taille, 0);
    if (count != taille)
        printf("couldn't send unrok");
    return NULL;
}

void *send_list(int sock, game *game_current)
{
    int size_list = sizeof(uint8_t) + SIZE_OF_HEAD + 1 + SIZE_OF_END + 1;
    char games_mess[size_list];
    memmove(games_mess, LISTS, SIZE_OF_HEAD);
    memmove(games_mess + SIZE_OF_HEAD, " ", 1);
    memmove(games_mess + SIZE_OF_HEAD + 1, &(game_current->game_id), sizeof(uint8_t));
    memmove(games_mess + SIZE_OF_HEAD + 1 + sizeof(uint8_t), END_TCP, SIZE_OF_END);
    if (size_list != send(sock, games_mess, size_list, 0))
        printf("Coudln't send list!");

    element *ptr = game_current->joueurs->first;
    for (int i = 0; i < game_current->joueurs->count; i++)
    {
        int taille = SIZE_OF_HEAD + 9 + SIZE_OF_END;
        char game_mess[taille + 1];
        memmove(game_mess, PLAYR, SIZE_OF_HEAD);
        memmove(game_mess + SIZE_OF_HEAD, " ", 1);
        memmove(game_mess + SIZE_OF_HEAD + 1, &(((joueur *)ptr->data)->id), 8);
        memmove(game_mess + SIZE_OF_HEAD + 9, END_TCP, SIZE_OF_END);
        if (taille != send(sock, games_mess, taille, 0))
            printf("Couldn't send PLAYR %d\n",i);
        ptr = ptr->next;
    }
    return NULL;
}