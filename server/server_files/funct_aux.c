#include "funct_aux.h"
#include "entity_position.h"

void *func_send_games(int sock2, listElements *games)
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
        uint8_t player_count = ((game *)ptr->data)->joueurs->count;
        int taille = (sizeof(uint8_t) * 2) + SIZE_OF_HEAD + 2 + SIZE_OF_END;
        char game_mess[taille];
        memmove(game_mess, OGAME, SIZE_OF_HEAD);
        memmove(game_mess + SIZE_OF_HEAD, " ", 1);
        memmove(game_mess + SIZE_OF_HEAD + 1, &gamed_id, sizeof(uint8_t));
        memmove(game_mess + SIZE_OF_HEAD + 1 + sizeof(uint8_t), " ", 1);
        memmove(game_mess + SIZE_OF_HEAD + 2 + sizeof(uint8_t), &player_count, sizeof(uint8_t));
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
    int taille = 14 + SIZE_OF_END;
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
    new->map = createMap(11, 11);
    new->widthMap = 11;
    new->heightMap = 11;
    printMap(new->map, new->heightMap, new->widthMap);
    if (games->count == 0)
    {
        new->game_id = 1;
    }
    else if (((game *)games->last->data)->game_id == games->count)
    {
        new->game_id = games->count + 1;
    }
    else
    {
        element *ptr = games->first;
        for (int i = 1; i <= games->count; i++)
        {
            if (i != ((game *)ptr->data)->game_id)
            {
                new->game_id = i;
                break;
            }
            ptr = ptr->next;
        }
    }
    new->joueurs = malloc(sizeof(listElements));
    addEl(new->joueurs, NULL, j1);
    addEl(games, games->last, new);
    j1->current = new;
    taille = SIZE_OF_END + 1 + sizeof(uint8_t) + SIZE_OF_HEAD;
    char message[taille];
    memmove(message, REGOK, SIZE_OF_HEAD);
    memmove(message + SIZE_OF_HEAD, " ", 1);
    memmove(message + SIZE_OF_HEAD + 1, &new->game_id, sizeof(uint8_t));
    memmove(message + SIZE_OF_HEAD + 1 + sizeof(uint8_t), END_TCP, SIZE_OF_END);
    if ((taille) != send(sock2, message, taille, 0))
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
        printf("register taille != Dunno\n");
        return NULL;
    }
    uint8_t id = *(uint8_t *)(buffer + 15);
    game *_game = get_game(games, id);
    if (!_game)
    {
        func_send_regno(sock2);
        printf("id = %d regno\n", id);
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

    taille = SIZE_OF_END + 1 + sizeof(uint8_t) + SIZE_OF_HEAD;
    char message[taille];
    memmove(message, REGOK, SIZE_OF_HEAD);
    memmove(message + SIZE_OF_HEAD, " ", 1);
    memmove(message + SIZE_OF_HEAD + 1, &_game->game_id, sizeof(uint8_t));
    memmove(message + SIZE_OF_HEAD + 1 + sizeof(uint8_t), END_TCP, SIZE_OF_END);
    if ((taille) != send(sock2, message, taille, 0))
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

void *func_unreg(joueur *joueur, listElements *games, int sock)
{
    // remove joueur from game and free mem
    element *game;

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
    int size_list = (sizeof(uint8_t) * 2) + SIZE_OF_HEAD + 2 + SIZE_OF_END;
    char games_mess[size_list];
    memmove(games_mess, LISTS, SIZE_OF_HEAD);
    memmove(games_mess + SIZE_OF_HEAD, " ", 1);
    memmove(games_mess + SIZE_OF_HEAD + 1, &(game_current->game_id), sizeof(uint8_t));
    memmove(games_mess + SIZE_OF_HEAD + 1 + sizeof(u_int8_t), " ", 1);
    memmove(games_mess + SIZE_OF_HEAD + 2 + sizeof(u_int8_t), &(game_current->joueurs->count), sizeof(uint8_t));
    memmove(games_mess + SIZE_OF_HEAD + 2 + (sizeof(uint8_t) * 2), END_TCP, SIZE_OF_END);
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
        game_mess[taille] = '\0';
        printf("message sent %s\n", game_mess);
        if (taille != send(sock, games_mess, taille, 0))
            printf("Couldn't send PLAYR %d\n", i);
        ptr = ptr->next;
    }
    return NULL;
}

void *get_size_map(int sock, listElements *games)
{
    // [SIZE? m***]
    int taille = 1 + sizeof(uint8_t) + SIZE_OF_END;
    char buffer[taille];
    if (taille != recv(sock, buffer, taille, 0))
    {
        func_send_dunno(sock);
        return NULL;
    }
    uint8_t game_id = (uint8_t)buffer[1];
    game *el = get_game(games, game_id);
    if (el == NULL)
        func_send_dunno(sock);
    else
    {
        // send size
        int size = SIZE_OF_HEAD + 8 + SIZE_OF_END;
        char response[size];
        memmove(response, SIZES, SIZE_OF_HEAD);
        memmove(response + SIZE_OF_HEAD, " ", 1);
        memmove(response + SIZE_OF_HEAD + 1, &game_id, sizeof(uint8_t));
        memmove(response + SIZE_OF_HEAD + 1 + sizeof(uint8_t), " ", 1);
        memmove(response + SIZE_OF_HEAD + 2 + sizeof(uint8_t), &el->heightMap, sizeof(uint16_t));
        printf("Game id sent : %d and  height: %d\n", game_id, el->heightMap);
        memmove(response + SIZE_OF_HEAD + 2 + sizeof(uint8_t) + sizeof(uint16_t), " ", 1);
        memmove(response + SIZE_OF_HEAD + 3 + sizeof(uint8_t) + sizeof(uint16_t), &el->widthMap, sizeof(uint16_t));
        memmove(response + SIZE_OF_HEAD + 3 + sizeof(uint8_t) + sizeof(uint16_t) * 2, END_TCP, SIZE_OF_END);
        int count = send(sock, response, size, 0);
        if (count != size)
            printf("Impossible d'envoyer la taille de la map");
    }
    return NULL;
}
/*
void* send_welco(int sock,joueur *joueur){
    int taille = SIZE_OF_HEAD+SIZE_OF_END+31;
    // ip = 15, space = 6,port = 4, fantome+game_id = 2,size = 4
    char buffer[taille];
    memmove(buffer,WELCO,SIZE_OF_HEAD);
    memmove(buffer+SIZE_OF_HEAD," ",1);
    memmove(buffer+SIZE_OF_HEAD+1,joueur->current->game_id,sizeof(uint8_t));
    memmove(buffer+SIZE_OF_HEAD+1+sizeof(uint8_t)," ",1);
    memmove(buffer+SIZE_OF_HEAD+2+sizeof(uint8_t),joueur->current->heightMap,sizeof(uint16_t));
    memmove(buffer+SIZE_OF_HEAD+2+sizeof(uint8_t)+sizeof(uint16_t)," ",1);
    memmove(buffer+SIZE_OF_HEAD+3+sizeof(uint8_t)+sizeof(uint16_t),joueur->current->widthMap,sizeof(uint16_t));
    memmove(buffer+SIZE_OF_HEAD+3+sizeof(uint8_t)+(sizeof(uint16_t)*2)," ",1);
    memmove(buffer+SIZE_OF_HEAD+4+sizeof(uint8_t)+(sizeof(uint16_t)*2),joueur->current->fantomes->count,sizeof(uint8_t));
    if(taille!=send(sock,buffer,taille,0)){
        printf("Coudln't send WELCO\n");
    }
    return NULL;
}
void* send_posit(int sock,joueur *joueur){
    int taille =17+SIZE_OF_HEAD+SIZE_OF_END;

    return NULL;
}
/*
void* movPlayer(int sock, joueur *joueur){
    game *game=joueur->current;
    int taille = 9+SIZE_OF_END;
    char buffer[taille];
    if (taille != recv(sock, buffer, taille, 0)){
        func_send_dunno(sock);
        return NULL;
    }
    char direction[5];
    int dir;
    memmove(direction, buffer, 5);
    if (strcmp(direction,"UPMOV")==0) dir=0;
    else if(strcmp(direction,"RIMOV")==0) dir=1;
    else if(strcmp(direction,"DOMOV")==0) dir=2;
    else if(strcmp(direction,"LEMOV")==0) dir=3;
    else {
        //TODO: send dunno?
        func_send_dunno(sock);
        return NULL;
    }

    char distance[3];
    memmove(distance, buffer+6, 3);

    player_move(&game, dir, *joueur, atoi(distance));


    //TODO: check if their are ghost
    //TODO: [MOVEF x y p***]
    //TODO: use lock
    int x_joueur=joueur->x;
    int y_joueur=joueur->y;

    //TODO: refactor
    char x_res[3];
    if (x_joueur<100) sprintf(x_res, "0%d", x_joueur);
    else if(x_joueur<10 && x_joueur>=0) sprintf(x_res, "00%d", x_joueur);
    else sprintf(x_res, "%d", x_joueur);

    char y_res[3];
    if (x_joueur<100) sprintf(y_res, "0%d", y_joueur);
    else if(x_joueur<10 && x_joueur>=0) sprintf(y_res, "00%d", y_joueur);
    else sprintf(y_res, "%d", y_joueur);

    //[MOVE! x y ***]
    int size_list = SIZE_OF_HEAD+6+SIZE_OF_END;
    char games_mess[size_list];
    memmove(games_mess, MOVES, SIZE_OF_HEAD);
    memmove(games_mess + SIZE_OF_HEAD, " ", 1);
    memmove(games_mess + SIZE_OF_HEAD + 1, x_res, 3);
    memmove(games_mess + SIZE_OF_HEAD + 4, " ", 1);
    memmove(games_mess + SIZE_OF_HEAD + 5, y_res, 3);
    memmove(games_mess + SIZE_OF_HEAD + 8, END_TCP, SIZE_OF_END);
    if (size_list != send(sock, games_mess, size_list, 0))
        printf("Coudln't send list!");

}*/