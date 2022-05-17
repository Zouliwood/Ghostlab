#include "funct_aux.h"

extern pthread_mutex_t verrou;

void func_send_games(int sock2, listElements *games)
{
    uint8_t games_waiting = 0;
    pthread_mutex_lock(&verrou);
    element *ptr = games->first;
    pthread_mutex_unlock(&verrou);
    for (int i = 0; i < getListCount(games); i++)
    {
        if (((game *)ptr->data)->encours == -1)
            games_waiting++;
        pthread_mutex_lock(&verrou);
        ptr = ptr->next;
        pthread_mutex_unlock(&verrou);
    }
    int size_games = sizeof(uint8_t) + SIZE_OF_HEAD + SIZE_OF_END + 1;
    char games_mess[size_games];
    memmove(games_mess, GAMES, SIZE_OF_HEAD);
    memmove(games_mess + SIZE_OF_HEAD, " ", 1);
    memmove(games_mess + SIZE_OF_HEAD + 1, &games_waiting, sizeof(uint8_t));
    memmove(games_mess + SIZE_OF_HEAD + 1 + sizeof(uint8_t), END_TCP, SIZE_OF_END);
    if (size_games != send(sock2, games_mess, size_games, 0))
    {
        printf("Couldn't send GAMES");
    }
    pthread_mutex_lock(&verrou);
    ptr = games->first;
    pthread_mutex_unlock(&verrou);
    for (int i = 0; i < getListCount(games); i++)
    {
        if (((game *)ptr->data)->encours == -1)
        {
            uint8_t gamed_id = ((game *)ptr->data)->game_id;
            uint8_t player_count = getListCount(((game *)ptr->data)->joueurs);
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
            pthread_mutex_lock(&verrou);
            ptr = ptr->next;
            pthread_mutex_unlock(&verrou);
        }
    }
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
    j1->x = -1;
    j1->y = -1;
    // création de la partie
    game *new = malloc(sizeof(game));
    new->encours = -1;
    new->start = 0;
    srand(time(NULL));
    int w = rand() % 20 + 11;
    int h = rand() % 20 + 11;
    new->map = createMap(h, w);
    new->widthMap = w;
    new->heightMap = h;
    new->joueurs = malloc(sizeof(listElements));
    new->joueurs->count = 0;
    new->joueurs->first = NULL;
    new->joueurs->last = NULL;
    new->fantomes = malloc(sizeof(listElements));
    new->fantomes->count = 0;
    new->fantomes->first = NULL;
    new->fantomes->last = NULL;
    memset(new->ip, '#', 15);
    memmove(new->ip, "225.1.2.4", 9);
    initGhost(new);

    // création du game_id
    if (getListCount(games) == 0)
    {
        new->game_id = 1;
    }
    else if (((game *)games->last->data)->game_id == getListCount(games))
    {
        new->game_id = getListCount(games) + 1;
    }
    else
    {
        pthread_mutex_lock(&verrou);
        element *ptr = games->first;
        pthread_mutex_unlock(&verrou);
        for (int i = 1; i <= getListCount(games); i++)
        {
            if (i != ((game *)ptr->data)->game_id)
            {
                new->game_id = i;
                break;
            }
            pthread_mutex_lock(&verrou);
            ptr = ptr->next;
            pthread_mutex_unlock(&verrou);
        }
    }
    new->port = (1023 + new->game_id);
    new->sock_udp = socket(AF_INET, SOCK_DGRAM, 0);
    // ajout du premier joueur
    addEl(new->joueurs, NULL, j1);
    // ajout de la partie dans games
    addEl(games, games->last, new);
    // le joueurs pointe vers la partie créé
    j1->current = new;
    // envoie du REGOK
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
    // block if game already started
    if (!_game || _game->encours == 1)
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

void func_send_dunno(int sock2)
{
    int taille = SIZE_OF_HEAD + SIZE_OF_END;
    char message[taille];
    memmove(message, DUNNO, SIZE_OF_HEAD);
    memmove(message + SIZE_OF_HEAD, END_TCP, SIZE_OF_END);
    int count = send(sock2, message, taille, 0);
    if (count != taille)
        printf("Couldn't send duno");
}

void func_send_regno(int sock2)
{
    int taille = SIZE_OF_HEAD + SIZE_OF_END;
    char message[taille];
    memmove(message, REGNO, SIZE_OF_HEAD);
    memmove(message + SIZE_OF_HEAD, END_TCP, SIZE_OF_END);
    int count = send(sock2, message, taille, 0);
    if (count != taille)
        printf("Couldn't send regno");
}

void start_game(joueur *joueur, int socket)
{
    char end_start[4];
    int count = recv(socket, end_start, SIZE_OF_END, 0);
    end_start[count] = '\0';
    if (strcmp(end_start, END_TCP) != 0)
    {
        func_send_dunno(socket);
        printf("Wrong input for start");
    }
    else
    {
        if (joueur->current == NULL)
        {
            func_send_dunno(socket);
        }
        pthread_mutex_lock(&verrou);
        joueur->current->start += 1;
        pthread_mutex_unlock(&verrou);
        init_pos_player(joueur, joueur->current);
        if (getGameStart(joueur->current) > 1 && getGameStart(joueur->current) == getListCount(joueur->current->joueurs))
        {
            pthread_mutex_lock(&verrou);
            joueur->current->encours = 1;
            pthread_mutex_unlock(&verrou);
            printMap(joueur->current->map, joueur->current->heightMap, joueur->current->widthMap);
        }
    }
}

void *func_unreg(joueur *joueur, listElements *games, int sock)
{
    // remove joueur from game and free mem
    uint8_t id = joueur->current->game_id;
    pthread_mutex_lock(&verrou);
    element *game_ptr = games->first;
    pthread_mutex_unlock(&verrou);
    for (int i = 0; i < getListCount(games); i++)
    {
        if (((game *)game_ptr->data)->game_id == id)
        {
            break;
        }
        pthread_mutex_lock(&verrou);
        game_ptr = game_ptr->next;
        pthread_mutex_unlock(&verrou);
    }
    pthread_mutex_lock(&verrou);
    element *ptr = joueur->current->joueurs->first;
    pthread_mutex_unlock(&verrou);
    while (ptr->data != joueur)
    {
        pthread_mutex_lock(&verrou);
        ptr = ptr->next;
        pthread_mutex_unlock(&verrou);
    }
    removeEl(joueur->current->joueurs, ptr);
    if (getListCount(joueur->current->joueurs) == 0)
    {
        freeGame((game *)game_ptr->data);
        removeEl(games, game_ptr);
    }
    free(joueur);
    // send unrok
    int taille = SIZE_OF_HEAD + SIZE_OF_END + 1 + sizeof(uint8_t);
    char message[taille];
    memmove(message, UNROK, SIZE_OF_HEAD);
    memmove(message + SIZE_OF_HEAD, " ", 1);
    memmove(message + SIZE_OF_HEAD + 1, &id, sizeof(uint8_t));
    memmove(message + SIZE_OF_HEAD + 1 + sizeof(uint8_t), END_TCP, SIZE_OF_END);
    int count = send(sock, message, taille, 0);
    if (count != taille)
        printf("couldn't send unrok");
    return NULL;
}

void send_list(int sock, game *game_current)
{
    int size_list = (sizeof(uint8_t) * 2) + SIZE_OF_HEAD + 2 + SIZE_OF_END;
    char games_mess[size_list];
    memmove(games_mess, LISTS, SIZE_OF_HEAD);
    memmove(games_mess + SIZE_OF_HEAD, " ", 1);
    memmove(games_mess + SIZE_OF_HEAD + 1, &(game_current->game_id), sizeof(uint8_t));
    memmove(games_mess + SIZE_OF_HEAD + 1 + sizeof(u_int8_t), " ", 1);
    uint8_t temp = getListCount(game_current->joueurs);
    memmove(games_mess + SIZE_OF_HEAD + 2 + sizeof(u_int8_t), &temp, sizeof(uint8_t));
    memmove(games_mess + SIZE_OF_HEAD + 2 + (sizeof(uint8_t) * 2), END_TCP, SIZE_OF_END);
    if (size_list != send(sock, games_mess, size_list, 0))
        printf("Coudln't send list!");
    pthread_mutex_lock(&verrou);
    element *ptr = game_current->joueurs->first;
    pthread_mutex_unlock(&verrou);
    for (int i = 0; i < getListCount(game_current->joueurs); i++)
    {
        int taille = SIZE_OF_HEAD + 9 + SIZE_OF_END;
        char game_mess[taille + 1];
        memmove(game_mess, PLAYR, SIZE_OF_HEAD);
        memmove(game_mess + SIZE_OF_HEAD, " ", 1);
        memmove(game_mess + SIZE_OF_HEAD + 1, &(((joueur *)ptr->data)->id), 8);
        memmove(game_mess + SIZE_OF_HEAD + 9, END_TCP, SIZE_OF_END);
        game_mess[taille] = '\0';
        if (taille != send(sock, game_mess, taille, 0))
            printf("Couldn't send PLAYR %d\n", i);
        pthread_mutex_lock(&verrou);
        ptr = ptr->next;
        pthread_mutex_unlock(&verrou);
    }
}

void get_size_map(int sock, listElements *games)
{
    // [SIZE? m***]
    int taille = 1 + sizeof(uint8_t) + SIZE_OF_END;
    char buffer[taille];
    if (taille != recv(sock, buffer, taille, 0))
    {
        func_send_dunno(sock);
    }
    uint8_t game_id = (uint8_t)buffer[1];
    game *el = get_game(games, game_id);
    if (el == NULL)
        func_send_dunno(sock);
    else
    {
        // send size
        int size = SIZE_OF_HEAD + 3 + sizeof(uint8_t) + (sizeof(uint16_t) * 2) + SIZE_OF_END;
        char response[size];
        memmove(response, SIZES, SIZE_OF_HEAD);
        memmove(response + SIZE_OF_HEAD, " ", 1);
        memmove(response + SIZE_OF_HEAD + 1, &game_id, sizeof(uint8_t));
        memmove(response + SIZE_OF_HEAD + 1 + sizeof(uint8_t), " ", 1);
        uint16_t h =htole16(el->heightMap);
        memmove(response + SIZE_OF_HEAD + 2 + sizeof(uint8_t), &h, sizeof(uint16_t));
        memmove(response + SIZE_OF_HEAD + 2 + sizeof(uint8_t) + sizeof(uint16_t), " ", 1);
        uint16_t w =htole16(el->widthMap);
        memmove(response + SIZE_OF_HEAD + 3 + sizeof(uint8_t) + sizeof(uint16_t),&w, sizeof(uint16_t));
        memmove(response + SIZE_OF_HEAD + 3 + sizeof(uint8_t) + sizeof(uint16_t) * 2, END_TCP, SIZE_OF_END);
        int count = send(sock, response, size, 0);
        if (count != size)
            printf("Impossible d'envoyer la taille de la map");
    }
}

void send_welco(int sock, joueur *player)
{
    int taille = SIZE_OF_HEAD + SIZE_OF_END + 16 + (sizeof(uint8_t) * 2) + (sizeof(uint16_t) * 2) + 4 + 5;
    char welco_mess[taille];
    if (player == NULL || player->id == NULL || player->port == NULL)
    {
        printf("Error wrong value for joueurs");
    }
    else
    {
        memmove(welco_mess, WELCO, SIZE_OF_HEAD);
        memmove(welco_mess + SIZE_OF_HEAD, " ", 1);
        memmove(welco_mess + SIZE_OF_HEAD + 1, &player->current->game_id, sizeof(uint8_t));
        memmove(welco_mess + SIZE_OF_HEAD + 1 + sizeof(uint8_t), " ", 1);
        uint16_t h=htole16(player->current->heightMap);
        memmove(welco_mess + SIZE_OF_HEAD + 2 + sizeof(uint8_t), &h, sizeof(uint16_t));
        memmove(welco_mess + SIZE_OF_HEAD + 2 + sizeof(uint8_t) + sizeof(uint16_t), " ", 1);
        uint16_t w=htole16(player->current->widthMap);
        memmove(welco_mess + SIZE_OF_HEAD + 3 + sizeof(uint8_t) + sizeof(uint16_t), &w, sizeof(uint16_t));
        memmove(welco_mess + SIZE_OF_HEAD + 3 + sizeof(uint8_t) + (sizeof(uint16_t) * 2), " ", 1);
        uint8_t temp = getListCount(player->current->fantomes);
        memmove(welco_mess + SIZE_OF_HEAD + 4 + sizeof(uint8_t) + (sizeof(uint16_t) * 2), &temp, sizeof(uint8_t));
        memmove(welco_mess + SIZE_OF_HEAD + 4 + (sizeof(uint8_t) * 2) + (sizeof(uint16_t) * 2), " ", 1);
        memmove(welco_mess + SIZE_OF_HEAD + 5 + (sizeof(uint8_t) * 2) + (sizeof(uint16_t) * 2), player->current->ip, 15);
        memmove(welco_mess + SIZE_OF_HEAD + 20 + (sizeof(uint8_t) * 2) + (sizeof(uint16_t) * 2), " ", 1);
        sprintf(welco_mess + SIZE_OF_HEAD + 21 + (sizeof(uint8_t) * 2) + (sizeof(uint16_t) * 2), "%04d", player->current->port);
        memmove(welco_mess + SIZE_OF_HEAD + 25 + (sizeof(uint8_t) * 2) + (sizeof(uint16_t) * 2), END_TCP, SIZE_OF_END);
        if (taille != send(sock, welco_mess, taille, 0))
        {
            printf("Coudln't send WELCO\n");
        }
        send_posit(sock, player);
    }
}

void send_posit(int sock, joueur *joueur)
{
    int taille = SIZE_OF_HEAD + 17 + SIZE_OF_END;
    char reponse[taille + 1];
    memmove(reponse, POSIT, SIZE_OF_HEAD);
    memmove(reponse + SIZE_OF_HEAD, " ", 1);
    memmove(reponse + SIZE_OF_HEAD + 1, joueur->id, 8);
    memmove(reponse + SIZE_OF_HEAD + 9, " ", 1);
    sprintf(reponse + SIZE_OF_HEAD + 10, "%03d", joueur->x);
    memmove(reponse + SIZE_OF_HEAD + 13, " ", 1);
    sprintf(reponse + SIZE_OF_HEAD + 14, "%03d", joueur->y);
    memmove(reponse + SIZE_OF_HEAD + 17, END_TCP, SIZE_OF_END);
    reponse[taille] = '\0';
    printf("%s\n", reponse);
    if (taille != send(sock, reponse, taille, 0))
    {
        printf("Coudln't send POSIT\n");
    }
}

void movPlayer(int sock, int dir, joueur *joueur, listElements *games)
{
    game *game = joueur->current;
    int taille = 1 + 3 + SIZE_OF_END;
    char buffer[taille];
    if (taille != recv(sock, buffer, taille, 0))
    {
        printf("Wrong size for mov\n");
        func_send_dunno(sock);
    }

    char endTCP[4];
    memmove(endTCP, buffer + 4, 3);
    endTCP[3] = '\0';
    if (strcmp(endTCP, END_TCP) != 0)
    {
        func_send_dunno(sock);
        printf("Wrong format for mov\n");
    }

    char distance[3];
    memmove(distance, buffer + 1, 3);
    player_move(game, dir, joueur, atoi(distance));

    //[MOVE! x y ***]
    taille = SIZE_OF_HEAD + 2 + 6 + SIZE_OF_END;
    char games_mess[taille + 1];
    memmove(games_mess, MOVES, SIZE_OF_HEAD);
    memmove(games_mess + SIZE_OF_HEAD, " ", 1);
    sprintf(games_mess + SIZE_OF_HEAD + 1, "%03d", joueur->x);
    memmove(games_mess + SIZE_OF_HEAD + 4, " ", 1);
    sprintf(games_mess + SIZE_OF_HEAD + 5, "%03d", joueur->y);
    memmove(games_mess + SIZE_OF_HEAD + 8, END_TCP, SIZE_OF_END);
    games_mess[taille] = '\0';
    printf("%s\n", games_mess);
    if (taille != send(sock, games_mess, taille, 0))
        printf("Coudln't send list!");
    if (game->fantomes->count == 0)
    {
        // TODO mutlicast
        pthread_mutex_lock(&verrou);
        joueur->current->encours = 1;
        pthread_mutex_unlock(&verrou);
    }
    else
        movGhost(game);
}

void quit_game(int sock, joueur *player, listElements *games)
{
    // execution du iquit
    if (!player->current)
    {
        int joueurs_in_game = getListCount(player->current->joueurs);
        if (joueurs_in_game == 1)
        {
            removeEl(player->current->joueurs, player->current->joueurs->first);
            pthread_mutex_lock(&verrou);
            element *game_ptr = games->first;
            pthread_mutex_unlock(&verrou);
            for (int i = 0; i < getListCount(games); i++)
            {
                if (((game *)game_ptr->data)->game_id == player->current->game_id)
                {
                    break;
                }
                pthread_mutex_lock(&verrou);
                game_ptr = game_ptr->next;
                pthread_mutex_unlock(&verrou);
            }
            removeEl(games, game_ptr);
            freeGame(player->current);
        }
        else
        {
            pthread_mutex_lock(&verrou);
            element *ptr = player->current->joueurs->first;
            pthread_mutex_unlock(&verrou);
            for (int i = 0; i < joueurs_in_game; i++)
            {
                if (ptr->data == player)
                    break;
                else
                    pthread_mutex_lock(&verrou);
                ptr = ptr->next;
                pthread_mutex_unlock(&verrou);
            }
            removeEl(player->current->joueurs, ptr);
        }
    }
    free(player);

    // send goodbye
    char goodbye[SIZE_OF_HEAD + SIZE_OF_END];
    memmove(goodbye, GOBYE, SIZE_OF_HEAD);
    memmove(goodbye + SIZE_OF_HEAD, END_TCP, SIZE_OF_END);
    if ((SIZE_OF_HEAD + SIZE_OF_END) != send(sock, goodbye, SIZE_OF_END + SIZE_OF_HEAD, 0))
    {
        printf("Couldn't send GOBYE\n");
    }
}

void send_glis(int sock, joueur *player)
{
    game *game_current = player->current;
    int taille = SIZE_OF_HEAD + 1 + SIZE_OF_END + sizeof(uint8_t);
    char glis_mess[taille];
    memmove(glis_mess, GLISS, SIZE_OF_HEAD);
    memmove(glis_mess + SIZE_OF_HEAD, " ", 1);
    uint8_t temp = getListCount(game_current->joueurs);
    memmove(glis_mess + SIZE_OF_HEAD + 1, &temp, sizeof(uint8_t));
    memmove(glis_mess + SIZE_OF_HEAD + 1 + sizeof(uint8_t), END_TCP, SIZE_OF_END);
    if (taille != send(sock, glis_mess, taille, 0))
        printf("Coudln't send glis!\n");
    pthread_mutex_lock(&verrou);
    element *ptr = game_current->joueurs->first;
    pthread_mutex_unlock(&verrou);
    for (int i = 0; i < getListCount(game_current->joueurs); i++)
    {
        taille = SIZE_OF_HEAD + 4 + 8 + 4 + 3 + 3 + SIZE_OF_END;
        // port+id+_+x+Y
        char gplyr_mess[taille];
        memmove(gplyr_mess, GPLYR, SIZE_OF_HEAD);
        memmove(gplyr_mess + SIZE_OF_HEAD, " ", 1);
        memmove(gplyr_mess + SIZE_OF_HEAD + 1, &(((joueur *)ptr->data)->id), 8);
        memmove(gplyr_mess + SIZE_OF_HEAD + 9, " ", 1);
        sprintf(gplyr_mess + SIZE_OF_HEAD + 10, "%03d", ((joueur *)ptr->data)->x);
        memmove(gplyr_mess + SIZE_OF_HEAD + 13, " ", 1);
        sprintf(gplyr_mess + SIZE_OF_HEAD + 14, "%03d", ((joueur *)ptr->data)->y);
        memmove(gplyr_mess + SIZE_OF_HEAD + 17, " ", 1);
        sprintf(gplyr_mess + SIZE_OF_HEAD + 18, "%04d", ((joueur *)ptr->data)->score);
        memmove(gplyr_mess + SIZE_OF_HEAD + 22, END_TCP, SIZE_OF_END);
        if (taille != send(sock, gplyr_mess, taille, 0))
            printf("Couldn't send GPLYR %d\n", i);
        pthread_mutex_lock(&verrou);
        ptr = ptr->next;
        pthread_mutex_unlock(&verrou);
    }
}

uint8_t getListCount(listElements *list)
{
    pthread_mutex_lock(&verrou);
    int res = list->count;
    pthread_mutex_unlock(&verrou);
    return res;
}

int getGameStart(game *game)
{
    pthread_mutex_lock(&verrou);
    int res = game->start;
    pthread_mutex_unlock(&verrou);
    return res;
}

int sendMess(int sock, joueur *me)
{
    int size_buffer = 200 + SIZE_OF_END + 8 + 2;
    char buffer[size_buffer];
    int count = recv(sock, buffer, size_buffer, 0);
    char id2[9];
    memmove(id2, buffer + 1, 8);
    id2[8] = '\0';
    pthread_mutex_lock(&verrou);
    element *ptr = me->current->joueurs->first;
    pthread_mutex_unlock(&verrou);
    for (int i = 0; i < getListCount(me->current->joueurs); i++)
    {
        char id[9];
        memmove(id,((joueur *)ptr->data)->id,8);
        id[8]='\0';
        if (strcmp(id, id2) == 0)
        {
            break;
        }
        if ((i + 1) == getListCount(me->current->joueurs))
        {
            return -1;
        }
        pthread_mutex_lock(&verrou);
        ptr = ptr->next;
        pthread_mutex_unlock(&verrou);
    }
    int size_message = count - 10 - SIZE_OF_END;
    char message[size_message];
    memmove(message, buffer + 10, size_message);

    struct addrinfo *first_info;
    struct addrinfo hints;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    // récuperation IP
    struct sockaddr_in addr;
    socklen_t addr_size = sizeof(struct sockaddr_in);
    if (getpeername(sock, (struct sockaddr *)&addr, &addr_size) == -1)
    {
        printf("Couldn't get ip\n");
        return -1;
    }
    struct in_addr temp;
    temp.s_addr = addr.sin_addr.s_addr;
    char *res = inet_ntoa(temp);
    if (res == NULL)
    {
        printf("Error while getting ip\n");
        return -1;
    }
    int r = getaddrinfo(res,((joueur *)ptr->data)->port, &hints, &first_info);
    if (r == 0)
    {
        if (first_info != NULL)
        {
            struct sockaddr *saddr = first_info->ai_addr;
            sendto(me->current->sock_udp,message,size_message,0,saddr,(socklen_t)sizeof(struct sockaddr_in));
            printf("J'ai envoyé le message udp\n");
        }
    }
    return 1;
}

void sendc(int sock,int true)
{
    int taille = SIZE_OF_END+SIZE_OF_HEAD;
    char mess[taille];
    if(true==1)
    {
        memmove(mess,"SEND!***",taille);
    }else{
        memmove(mess,"NSEND***",taille);
    } 
    if(taille!=send(sock, mess, taille, 0))
    {
        printf("Error while send send!\n");
    }  
}